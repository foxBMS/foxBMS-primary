# @copyright &copy; 2010 - 2017, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
#
# BSD 3-Clause License
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# We kindly request you to use one or more of the following phrases to refer to foxBMS in your hardware, software, documentation or advertising materials:
#
# &Prime;This product uses parts of foxBMS&reg;&Prime;
#
# &Prime;This product includes parts of foxBMS&reg;&Prime;
#
# &Prime;This product is derived from foxBMS&reg;&Prime;

"""Top dir WAF script
"""

from waflib import Logs, Utils, Context, Options
from waflib import Task, TaskGen
import os
import sys
import re
import datetime
import json
import platform
import subprocess
import ConfigParser


# console colors
color_p = '\x1b[35m'  # pink
color_n = '\x1b[0m'  # normal
color_c = '\x1b[36m'  # cyan

out = "build"

class IniFile:
    """ Read and store the WAF build configuration from the *.ini file.

    All attributs are public.

    Attributes:
        VERSION (str): Specifies the version passed as define to the
            compiler
        APPNAME (str):  Specifies the application name passed as define
            o the compiler
        VENDOR (str):   Specifies the application vendor (not further
            used)
        CFLAGS (str): compiler flags
        mainwscriptdir (str): unix-style path to directory contaioning
            the main wscript
        mainwscript (str): unix-style path to main wscript
        logfile (str):  name of file where build logging output is
            stored.
        envstore (str): name of file where enviroment varibales are
            stored.
        lock_file (bool): enables storage of waf-lock files in top dir.
        configheader (str): file name in which waf configured defines
            are stored.
        sizelog (str): name of file where size logging output is
            stored.
        chksumscript (str): unix-style path to checksum script
        chksuminifile (str): unix-style path to file containing
            addresses etc. for the checksum calcuation
        docdir (str): unix-style path to documentation directory
        sphinx (str): unix-style path to directory containing the
            sphinx documentation configuration and documentation
            sources.
        doxygendocdir (str): unix-style path to doxygen documentation
            directory.
        doctooldir (str): unix-style path to tool defintion of sphinx
            and doxygen.
        styleguidescript (str): unix-style path to script for
            stylechecking of *.c, and *.h files.
    """
    def __init__(self, configFile):
        """Sets path to waf configuration file as attribute.
        """
        self.cfgFile = configFile

    # function to parse alle needed informations from the config file, using Python ConfigParser module
    def ParseConfig(self):
        """Parsing of the configuration file
        """
        configOK = False

        Config = ConfigParser.ConfigParser()
        self.builddir = out
        try:
            Config.read(self.cfgFile)
        except:
            print('\nError! Ini-File not found in path \n' + os.getcwd())

        try:
            # [project]
            self.VERSION = Config.get('project', 'VERSION')
            o = 'Project Version:               ' + self.VERSION + '\n'

            self.APPNAME = Config.get('project', 'APPNAME')
            o += 'Project Application Name:      ' + self.APPNAME + '\n'
            self.VENDOR = Config.get('project', 'VENDOR')
            o += 'Project Vendor:                ' + self.VENDOR + '\n'

            # [cflags]
            temp = Config.get('cflags', 'CFLAGS')
            self.CFLAGS = temp.split(' ')
            o += 'CFLAGS:                        ' + temp + '\n'

            # [waf]
            temp = Config.get('waf', 'lock')
            self.lock_file = temp
            o += 'Generate lock file:            ' + self.lock_file + '\n'
   
            # [checksum]
            temp = Config.get('checksum', 'chksumscript')
            self.chksumscript = os.path.normpath(temp)
            o += 'Checksum tool:                 ' + self.chksumscript + '\n'
            temp = Config.get('checksum', 'chksuminifile')
            self.chksuminifile = os.path.normpath(temp)
            o += 'Checksum configuration:        ' + self.chksuminifile + '\n'

            # [documentation]
            temp = Config.get('documentation', 'docdir')
            self.docdir = os.path.normpath(temp)
            o += 'Documentation directory:       ' + self.docdir + '\n'

            temp = Config.get('documentation', 'sphinxdocdir')
            self.sphinxdocdir = os.path.normpath(temp)
            o += 'Sphinx directory:              ' + self.sphinxdocdir + '\n'

            temp = Config.get('documentation', 'doxygendocdir')
            self.doxygendocdir = os.path.normpath(temp)
            o += 'Sphinx directory:              ' + self.doxygendocdir + '\n'

            temp = Config.get('documentation', 'doctooldir')
            self.doctooldir = os.path.normpath(temp)
            o += 'Documentation tool directory:  ' + self.doctooldir + '\n'

            # styleguide
            temp = Config.get('styleguide', 'styleguidescript')
            self.styleguidescript = os.path.normpath(temp)
            o += 'Styleguide script:             ' + self.styleguidescript + '\n'

            configOK = True

        except AttributeError as e:
            print '\nConfig parsing Error\n' + os.getcwd()
            print e
            configOK = False
        self.info = o
        self.Derive()
        return configOK

    def Derive(self):
        """ Derives some paths needed for binary building and checksum
        tool.
        """
        self.configheader = self.APPNAME+'config.h'
        self.elffile = os.path.join('src', 'general', self.APPNAME+'.elf')
        self.hexfile = os.path.join('src', 'general', self.APPNAME+'.hex')
        self.binflash = os.path.join('src', 'general', self.APPNAME+'_flash.bin')
        self.binflashheader = os.path.join('src', 'general', self.APPNAME+'_flashheader.bin')
        self.elffilebd = os.path.join(self.builddir, self.elffile)
        self.hexfilebd = os.path.join(self.builddir, self.hexfile)
        self.binflashbd = os.path.join(self.builddir, self.binflash)
        self.binflashheaderbd = os.path.join(self.builddir, self.binflashheader)


# get project configurtion
Cfg = IniFile('waf.ini')
Cfg.ParseConfig()

# WAF features, functions, ... #

def options(opt):
    opt.load('compiler_c')
    opt.load(['doxygen', 'sphinx_build'], tooldir=Cfg.doctooldir)
    opt.add_option('-c', '--config', action='store', default=None, help='file containing additional configuration variables', dest='configfile')
    opt.add_option('-t', '--target', action='store', default='debug', help='build target: debug (default)/release', dest='target')


def loadConfigFile():
    _fname = Options.options.configfile
    if _fname is None:
        return
    json.load(_fname)

def configure(conf):
    """Waf function "configure"
    Invoked by: "python tools\waf-1.8.12 configure"

    Configures waf for building the sources and the documentation.
    After "configure" and the waf-lock files are generated, one is
    able to run "python tools\waf-1.8.12 build" or "python
    tools\waf-1.8.12 chksum" without calling "python tools\waf-1.8.12
    configure" first.
    """
    print Cfg.info
    loadConfigFile()
    # prefix for all gcc related tools
    pref = 'arm-none-eabi'

    if sys.platform.startswith('win'):
        conf.env.CC = pref + '-gcc.exe'
        conf.env.AR = pref + '-ar.exe'
        conf.find_program(pref + '-strip', var='STRIP')
        conf.env.LINK_CC = pref + '-g++.exe'
        conf.find_program(pref + '-objcopy', var='hexgen')
        conf.find_program(pref + '-size', var='SIZE')
        conf.find_program('python', var='PYTHON')
    else:
        conf.env.CC = pref + '-gcc'
        conf.env.AR = pref + '-ar'
        conf.env.LINK_CC = pref + '-g++'
        conf.find_program(pref + '-strip', var='STRIP')
        conf.find_program(pref + '-objcopy', var='hexgen')
        conf.find_program(pref + '-size', var='SIZE')
    conf.env.CFLAGS = Cfg.CFLAGS
    conf.env.CFLAGS += str('-DBUILD_VERSION=\"' + str(Cfg.VERSION) + '\"').split(' ')
    conf.env.CFLAGS += str('-DBUILD_APPNAME=\"' + str(Cfg.APPNAME) + '\"').split(' ')
    conf.env.CFLAGS += '-DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_ITM -DUSE_HAL_DRIVER -DHSE_VALUE=8000000'.split(' ')
    conf.load('compiler_c')
    conf.load(['doxygen', 'sphinx_build'])
    conf.find_program('git', mandatory=False)

    conf.env.version = Cfg.VERSION
    conf.env.appname = Cfg.APPNAME
    conf.env.vendor = Cfg.VENDOR
    conf.env.ldscript = os.path.join(conf.srcnode.abspath(), os.path.join('src', 'STM32F429ZIT6_FLASH.ld'))
    try:
        conf.env.buildno = conf.cmd_and_log(conf.env.GIT[0] + ' rev-parse --short HEAD').strip()
    except:
        conf.env.buildno = 'none'
    utcnow = datetime.datetime.utcnow()
    utcnow = ''.join(utcnow.isoformat('-').split('.')[0].replace(':', '-').split('-'))
    conf.env.timestamp = utcnow
    for k in 'gcc ar cpp ranlib as'.split():
        print pref + '-' + k, k.upper()
        conf.find_program(pref + '-' + k, var=k.upper(), mandatory=True)

    conf.define('BUILD_APPNAME', Cfg.APPNAME)
    conf.define('BUILD_VERSION', Cfg.VERSION)
    conf.define('BUILD_VENDOR', Cfg.VENDOR)
    conf.define('BUILD_LDSCRIPT', conf.env.ldscript)
    conf.define('BUILD_NUMBER', conf.env.buildno)
    conf.define('TOOLCHAIN_WAF_ENABLED', 1)
    conf.define('STM32F429xx', 1)
    conf.define('USE_DRIVER_HAL', 1)
    conf.define('INCLUDE_eTaskGetState', 1)
    conf.env.target = conf.options.target
    conf.env.EXT_CC += ['.S']

    env_debug = conf.env.derive()
    env_debug.detach()
    env_release = conf.env.derive()
    env_release.detach()

    # configuration for debug
    conf.setenv('debug', env_debug)
    conf.define('RELEASE', 1)
    conf.undefine('DEBUG')
    conf.env.CFLAGS += ['-g', '-O0']
    env_debug.basename = conf.env.appname + '-' + conf.env.version + '-' + conf.env.buildno + '-' + conf.env.timestamp + '-debug'
    env_debug.PREFIX = conf.env.appname + '-' + conf.env.version + '-' + conf.env.buildno + '-' + conf.env.timestamp

    # configuration for release
    conf.setenv('release', env_release)
    conf.env.CFLAGS += ['-O2']
    env_release.basename = conf.env.appname + '-' + conf.env.version + '-' + conf.env.buildno + '-' + conf.env.timestamp + '-release'
    env_release.PREFIX = conf.env.appname + '-' + conf.env.version + '-' + conf.env.buildno + '-' + conf.env.timestamp
    if conf.options.target == 'release':
        conf.setenv('', env_release)
    else:
        conf.setenv('', env_debug)

    env_release.store(os.path.join(Cfg.builddir, 'env-store.log'))
    conf.write_config_header(Cfg.configheader)

    if Cfg.lock_file is False:
        conf.env.NO_LOCK_IN_TOP = True
        conf.env.NO_LOCK_IN_RUN = True

    print 'Basename:    ', conf.env.basename
    print 'Prefix:      ', conf.env.PREFIX
    try:
        print 'CFLAGS:      ', conf.env.CFLAGS[0]
        for i, flag in enumerate(conf.env.CFLAGS):
            if i != 0:
                print '             ', flag
    except:
        print '\nno CFLAGS specified'

def cleanall(conf):
    removedFiles = 0
    if os.path.isdir(Cfg.builddir):
        for root, directories, filenames in os.walk(Cfg.builddir):
            clean_file_extensions = ['.a', '.o', '.h', '.elf', '.bin']
            to_clean_files = ''
            for ext in clean_file_extensions:
                to_clean_files += '\\' + ext + '|'
            to_clean_files = to_clean_files[:-1]
            s = re.compile(to_clean_files)
            for filename in filenames: 
                fp = os.path.join(root, filename)
                if s.match(os.path.splitext(fp)[1]):
                    os.remove(fp)
                    removedFiles += 1
    print color_p + 'successfully cleaned all ', clean_file_extensions, 'files (', removedFiles, ' files removed).' + color_n
    print 'run "waf configure" first, to be able to build again.'

def rem(conf):
    '''Removes the build directory if it exists
    Invoked by: "python tools\waf-1.8.12 rem"
    
    rem is not feature but a function and has to be called python tools/waf-1.8.12 configure rem.
    The 'build' directory and all subfolder and files are deleted by "rm -rf" on *nix and "del /s /q"
    on Windows.
    '''
    if os.path.isdir(Cfg.builddir):
        if sys.platform.startswith('win'):
            cmd = 'rmdir /s /q ' + Cfg.builddir
            print cmd
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
            out, err = p.communicate()
            if out:
                print out
            if err:
                print err
            cmd = 'del /s /q ' + 'build.log'
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
            out, err = p.communicate()
            if out:
                print out
            if err:
                print err
        else:
            cmd = 'rm -rf ' + Cfg.builddir
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
            out, err = p.communicate()
            if out:
                print out
            if err:
                print err
            cmd = 'rm -rf ' + 'build.log'
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
            out, err = p.communicate()
            if out:
                print out
            if err:
                print err
        print 'Successfully cleaned build dir and log files.'
    else:
        print 'Nothing to clean.'


def build(bld):
    """Waf function "rem"
    Invoked by: "python tools\waf-1.8.12 build"

    A wscript with a function "build" must exists in every sub-directory that
    is built. The build instructions for the sub-directories have to be
    specified in the wscripts in the subdirectories.
    """
    import sys
    import logging
    from waflib import Logs

    # enables logging for build routine
    bld.logger = Logs.make_logger('build.log', Cfg.builddir)
    hdlr = logging.StreamHandler(sys.stdout)
    formatter = logging.Formatter('%(message)s')
    hdlr.setFormatter(formatter)
    bld.logger.addHandler(hdlr)
    bld.recurse('src')


def dist(conf):
    """Waf function "rem"
    Invoked by: "python tools\waf-1.8.12 dist"

    Packs the current status of the project in a tar.gz file
    """
    conf.base_name = 'foxbms'
    conf.algo = 'tar.gz'
    conf.excl = ' Packages workspace **/.waf-1* **/*~ **/*.pyc **/*.swp **/.lock-w* **/env.txt **/log.txt **/.git **/build **/*.tar.gz **/.gitignore **/tools/waf-1.8.12-*'

# START CHKSUM TASK DESCRIPTION
class chksum(Task.Task):
    """Waf function "size"
    Invoked by: "python tools\waf-1.8.12 size"

    Calculates the size of all libraries the foxbms.elf file.

    Gets all object files in the build directory (by file extension *.o) and the main foxbms.elf binary and processes
    the object with size in berkley format.
    """
    t = os.path.join(os.getcwd(), 'tools', 'waf-1.8.12')
    run_str = '${PYTHON} ' + t + ' chksum_function'
    color = 'CYAN'

def chksum_function(conf):
    """Waf function "chksum"
    Invoked by: "python tools\waf-1.8.12 chksum"

    Calculates the checksum of hexfile generated by the build process.
    This process needs to be called after "build".
    Calls the checksum with tool stored in tools/checksum/chksum.py
    with the configuration stored in tools/checksum/chksum.ini.

    Reads the returned checksum from the piped shell output.
    Writes the checksum back to the following files:
     - foxbms.hex,
     - foxbms.elf and
     - foxbms_flashheader.bin.
    """
    # Calculate checksum and write it back into foxbms.hex file
    tool = 'python'
    cmd = ' '.join([tool, Cfg.chksumscript, Cfg.chksuminifile, '-bd='+Cfg.builddir, '-hf='+Cfg.builddir+'/src/general/foxbms.hex'])
    print color_p + 'Subprocess: Calculating checksum from foxbms.hex\n' + color_c + cmd + color_n + '\n'
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    out, err = p.communicate()
    cs = (((out.split('* 32-bit SW-Chksum:     ')[1]).split('*'))[0].strip())
    print 'checksum output:\n----------------\n', out
    print 'Err:', err, '\n'

    # write checksum into foxbms.elf file
    tool = 'arm-none-eabi-gdb'
    cmd = '%s -q -se=%s --write -ex="set var ver_sw_validation.Checksum_u32 =%s" -ex="print ver_sw_validation.Checksum_u32" -ex="quit"' % (tool, Cfg.elffilebd, cs)
    print color_p + 'Subprocess: Writing into foxbms.elf\n' + color_c + cmd + color_n + '\n'
    print 'gdb output:\n-----------'
    p = subprocess.Popen(cmd, shell=True)
    p.wait()

    # write checksum into foxbms_flashheader.bin file
    tool = 'arm-none-eabi-objcopy -v'
    cmd = ' '.join([tool, '-j', '.flashheader', '-O', 'binary', Cfg.elffilebd, Cfg.binflashheaderbd])
    print '\n' + color_p + 'Subprocess: Writing into foxbms_flashheader.bin\n' + color_c + cmd + color_n + '\n'
    print 'objcopy output:\n---------------'
    p = subprocess.Popen(cmd, shell=True)
    p.wait()

@TaskGen.feature('chksum')
@TaskGen.after('hexgen')
def add_chksum_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('chksum', link_task.outputs[0])
# END CHKSUM TASK DESCRIPTION

def flash(bld):
    """Waf function "flash"
    Invoked by: "python tools\waf-1.8.12 flash"
    """
    subprocess.call("python tools/flashtool/stm32_loader.py -p COM10 -e -w -v -a 0x08000000 " + bld.path.abspath() + ("/build/src/general/foxbms_flash.bin"), shell=True)
    subprocess.call("python tools/flashtool/stm32_loader.py -p COM10 -w -v -a 0x080FFF00 " + bld.path.abspath() + ("/build/src/general/foxbms_flashheader.bin"), shell=True)


def styleguide(conf):
    """Waf function "styleguide"
    Invoked by: "python tools\waf-1.8.12 styleguide"

    Checks  *.c *.h files of the foxBMS project located in src/
    The output of the styleguide is put into build/styleguide.log
    """
    tool = 'python'
    cmd = ' '.join([tool, Cfg.styleguidescript])
    print '\n' + color_p + 'Subprocess: Checking foxBMS codestlye guidelines\n' + color_c + cmd + color_n + '\n'
    print 'styleguide output:\n---------------'
    p = subprocess.Popen(cmd, shell=True)
    p.wait()


def doxygen(bld):
    """Waf function "doxygen"
    Invoked by: "python tools\waf-1.8.12 doxygen"

    Builds the sphinx documentation defined in src/. For configuration
    (e.g., exlcuded files) see doc/doxygen/doxygen.conf.
    """
    if bld.env.DOXYGEN:
        _docbuilddir = os.path.join(bld.bldnode.abspath(), Cfg.docdir)
        if not os.path.exists(_docbuilddir):
            os.makedirs(_docbuilddir)
        doxygenconf = os.path.join(Cfg.doxygendocdir, 'doxygen.conf')
        bld(features="doxygen", doxyfile=doxygenconf)


def sphinx(bld):
    """Waf function "sphinx"
    Invoked by: "python tools\waf-1.8.12 sphinx"

    Builds the sphinx documentation defined in doc/sphinx.
    For configuration see doc/sphinx/conf.py.
    """
    bld.recurse(Cfg.sphinxdocdir)

# START SIZE TASK DESCRIPTION
class size(Task.Task):
    """Waf function "size"
    Invoked by: "python tools\waf-1.8.12 size"

    Calculates the size of all libraries the foxbms.elf file.

    Gets all object files in the build directory (by file extension *.o) and the main foxbms.elf binary and processes
    the object with size in berkley format.
    """
    t = os.path.join(os.getcwd(), 'tools', 'waf-1.8.12')
    run_str = '${PYTHON} ' + t + ' size_function'
    color = 'CYAN'

def size_function(conf):
    objlist = ''
    for root, dirs, files in os.walk(os.path.join(Cfg.builddir)):
        for file in files:
            if file.endswith('.o'):
                bpath = os.path.join(root, file)
                objlist += " " + os.path.join(bpath)
    for root, dirs, files in os.walk(os.path.join(Cfg.builddir)):
        for file in files:
            if file.endswith('.a'):
                bpath = os.path.join(root, file)
                objlist += " " + os.path.join(bpath)
    for root, dirs, files in os.walk(os.path.join(Cfg.builddir)):
        for file in files:
            if file.endswith('.elf'):
                bpath = os.path.join(root, file)
                objlist += " " + os.path.join(bpath)
    size_log_file = os.path.join(out, 'size.log')
    cmd = 'arm-none-eabi-size --format=berkley ' + objlist + ' > ' + size_log_file
    print color_c + cmd + color_n
    p = subprocess.Popen(cmd, shell=True)
    p.wait()
    cmd='type ' + size_log_file
    p = subprocess.Popen(cmd, shell=True)
    p.wait()

@TaskGen.feature('size')
@TaskGen.after('hexgen')
def add_size_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('size', link_task.outputs[0])
# END SIZE TASK DESCRIPTION

class strip(Task.Task):
    """Task generation: waf instructions for running
    arm-none-eabi-strip during release build
    """
    run_str = '${STRIP} ${SRC}'
    color = 'BLUE'


@TaskGen.feature('strip')
@TaskGen.after('apply_link')
def add_strip_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('strip', link_task.outputs[0])


class hexgen(Task.Task):
    run_str = '${hexgen} -O ihex ${SRC} ' + Cfg.hexfile
    color = 'CYAN'


@TaskGen.feature('hexgen')
@TaskGen.after('apply_link')
def add_hexgen_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('hexgen', link_task.outputs[0])


class binflashheadergen(Task.Task):
    """Task generation: converts .elf to .bin
    only flashheader
    """
    run_str = '${hexgen} -j .flashheader -O binary ${SRC} ' + Cfg.binflashheader
    color = 'RED'


class binflashgen(Task.Task):
    """Task generation: converts .elf to .bin
    only flash
    """
    run_str = '${hexgen} -R .bkp_ramsect -R .flashheader -O binary ${SRC} ' + Cfg.binflash
    color = 'RED'


@TaskGen.feature('bingen')
@TaskGen.after('apply_link')
def add_bingen_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('binflashgen', link_task.outputs[0])
    self.create_task('binflashheadergen', link_task.outputs[0])


import waflib.Tools.asm  # import before redefining
from waflib.TaskGen import extension


@extension('.S')
def asm_hook(self, node):
    """ Task generation: waf instructions for startup script compile
    routine
    """
    name = 'Sasm'
    out = node.change_ext('.o')
    task = self.create_task(name, node, out)
    try:
                self.compiled_tasks.append(task)
    except AttributeError:
                self.compiled_tasks = [task]
    return task


class Sasm(Task.Task):
    color = 'BLUE'
    run_str = '${CC} ${CFLAGS} ${CPPPATH_ST:INCPATHS} -DHSE_VALUE=8000000 -MMD -MP -MT${TGT} -c -x assembler -o ${TGT} ${SRC}'

# vim: set ft=python :
