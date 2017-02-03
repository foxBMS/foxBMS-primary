# @copyright &copy; 2010 - 2016, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
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

from waflib import Logs, Utils, Context, Options
from waflib import Task, TaskGen
import os
import sys
import datetime
import json
import platform
import subprocess

# Version and AppName are passed to compiler, string lengths are limited to 15 chararacters
VERSION = "0.4.3"
APPNAME = "foxbms"
VENDOR = 'Fraunhofer IISB'
LDSCRIPT = os.path.join(*['src', 'STM32F429ZIT6_FLASH.ld'])
SSCRIPT = os.path.join(*['src', 'general', 'config', 'startup_stm32f429xx.S'])

# console colors
color_p = '\x1b[35m' # pink
color_n = '\x1b[0m' # normal
color_c = '\x1b[36m' # cyan

def options(opt):
    opt.load('compiler_c')
    opt.load(['doxygen', 'sphinx_build'], tooldir=[os.path.join(*['tools', 'waftools'])])
    opt.add_option('-c', '--config', action='store', default=None, help='file containing additional configuration variables', dest='configfile')
    opt.add_option('-t', '--target', action='store', default='debug', help='build target: debug (default)/release', dest='target')
    opt.add_option('--enable-latex', action='store_true', default=False, help='enable latex->pdf generation of quickstart guide', dest='enable_latex')

def loadConfigFile():
    _fname = Options.options.configfile
    if _fname is None:
        return
    json.load(_fname)

def configure(conf):

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
    else:
        conf.env.CC = pref + '-gcc'
        conf.env.AR = pref + '-ar'
        conf.env.LINK_CC = pref + '-g++'
        conf.find_program(pref + '-strip', var='STRIP')
        conf.find_program(pref + '-objcopy', var='hexgen')
        conf.find_program(pref + '-size', var='SIZE')
    conf.env.CFLAGS = '-mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -fmessage-length=0 -fno-common -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -std=c99'.split(' ')
    conf.env.CFLAGS += str('-DBUILD_VERSION=\"' + str(VERSION) + '\"').split(' ')
    conf.env.CFLAGS += str('-DBUILD_APPNAME=\"' + str(APPNAME) + '\"').split(' ')
    conf.env.CFLAGS += '-DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_ITM -DUSE_HAL_DRIVER -DHSE_VALUE=8000000'.split(' ')
    conf.load('compiler_c')
    conf.load(['doxygen', 'sphinx_build'])
    if conf.options.enable_latex:
        conf.load('tex')
        if not conf.env.LATEX:
            conf.fatal('The program LaTeX is required')
    conf.find_program('git', mandatory=False)

    conf.env.version = VERSION
    conf.env.appname = APPNAME
    conf.env.vendor = VENDOR
    conf.env.ldscript = os.path.join(conf.srcnode.abspath(), LDSCRIPT)
    conf.env.sscript = os.path.join(conf.srcnode.abspath(), SSCRIPT)
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

    conf.define('BUILD_APPNAME', APPNAME)
    conf.define('BUILD_VERSION', VERSION)
    conf.define('BUILD_VENDOR', VENDOR)
    conf.define('BUILD_LDSCRIPT', conf.env.ldscript)
    conf.define('BUILD_NUMBER', conf.env.buildno)
    conf.define('TOOLCHAIN_WAF_ENABLED', 1)
    conf.define('STM32F429xx', 1)
    conf.define('USE_DRIVER_HAL', 1)
    conf.define('INCLUDE_eTaskGetState', 1)
    conf.env.target = conf.options.target
    conf.env.EXT_CC += ['.S']

    conf.env.generalincludes = ' '.join([
        os.path.join(conf.bldnode.abspath()),
        os.path.join(conf.srcnode.abspath(), 'src', 'engine', 'config'),
        os.path.join(conf.srcnode.abspath(), 'src', 'engine', 'database'),
        os.path.join(conf.srcnode.abspath(), 'src', 'engine', 'task'),
        os.path.join(conf.srcnode.abspath(), 'src', 'engine', 'sysctrl'),
        os.path.join(conf.srcnode.abspath(), 'src', 'general'),
        os.path.join(conf.srcnode.abspath(), 'src', 'general', 'config'),
        os.path.join(conf.srcnode.abspath(), 'src', 'general', 'includes'),
        os.path.join(conf.srcnode.abspath(), 'src', 'hal', 'CMSIS', 'Device', 'ST', 'STM32F4xx', 'Include'),
        os.path.join(conf.srcnode.abspath(), 'src', 'hal', 'CMSIS', 'Include'),
        os.path.join(conf.srcnode.abspath(), 'src', 'hal', 'STM32F4xx_HAL_Driver', 'Inc'),
        os.path.join(conf.srcnode.abspath(), 'src', 'hal', 'STM32F4xx_HAL_Driver', 'Inc', 'Legacy'),
        os.path.join(conf.srcnode.abspath(), 'src', 'module', 'cpu'),
        os.path.join(conf.srcnode.abspath(), 'src', 'module', 'io'),
        os.path.join(conf.srcnode.abspath(), 'src', 'module', 'config'),
        os.path.join(conf.srcnode.abspath(), 'src', 'module', 'contactor'),
        os.path.join(conf.srcnode.abspath(), 'src', 'module', 'rtc'),
        os.path.join(conf.srcnode.abspath(), 'src', 'engine', 'diag'),
        os.path.join(conf.srcnode.abspath(), 'src', 'module', 'eeprom'),
        os.path.join(conf.srcnode.abspath(), 'src', 'os'),
        os.path.join(conf.srcnode.abspath(), 'src', 'os', 'FreeRTOS', 'Source', 'CMSIS_RTOS'),
        os.path.join(conf.srcnode.abspath(), 'src', 'os', 'FreeRTOS', 'Source', 'include'),
        os.path.join(conf.srcnode.abspath(), 'src', 'os', 'FreeRTOS', 'Source', 'portable', 'GCC', 'ARM_CM4F'),
        ])

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

    env_release.store('env.txt')
    conf.write_config_header('foxbmsconfig.h')
    print conf.env.basename, conf.env.CFLAGS, conf.env.PREFIX



def build(bld):

    import sys, logging
    from waflib import Logs

    # enables logging for build routine
    bld.logger = Logs.make_logger('log.txt', 'build')
    hdlr = logging.StreamHandler(sys.stdout)
    formatter = logging.Formatter('%(message)s')
    hdlr.setFormatter(formatter)
    bld.logger.addHandler(hdlr)
    bld.recurse('src')


def dist(conf):
    conf.base_name = 'foxbms'
    conf.algo = 'tar.gz'
    conf.excl = ' **/.waf-1* **/*~ **/*.pyc **/*.swp **/.lock-w* **/env.txt **/log.txt **/.git **/build **/*.tar.gz **/.gitignore **/tools/waf-1.8.12-*'

def chksum(conf):
    '''Calculates the checksum of /foxbms/src/general/foxbms.hex

    Calls the checksum calcuation tool with the configuration stored in 'tools/checksum/chksum.ini'.
    Reads the returned checksum from the piped shell output.
    Writes the checksum back to the 
    - foxbms.hex,
    - foxbms.elf and
    - foxbms_flashheader.bin.
    
    chksum is not feature but a function and has to be called after build-feature.

    Args:
        conf

    Returns:
        -none-
    '''
    # Calculate checksum and write it back into foxbms.hex file
    tool = 'python'
    chkSumToolPath = os.path.join('tools', 'checksum', 'chksum.py')
    iniFilePath = os.path.join('tools', 'checksum', 'chksum.ini')
    cmd = ' '.join([tool, chkSumToolPath, iniFilePath])
    print color_p + 'Subprocess: Calculating checksum from foxbms.hex\n' + color_c + cmd + color_n + '\n'
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    out, err = p.communicate()
    cs = (((out.split('* 32-bit SW-Chksum:     ')[1]).split('*'))[0].strip())
    print 'checksum output:\n----------------\n', out
    print 'Error:', err, '\n'

    # write checksum into foxbms.elf file
    tool = 'arm-none-eabi-gdb'
    elfFilePath = os.path.join(*['build', 'src', 'general', 'foxbms.elf'])
    cmd = '%s -q -se=%s --write -ex="set var ver_sw_validation.Checksum_u32 =%s" -ex="print ver_sw_validation.Checksum_u32" -ex="quit"' % (tool, elfFilePath, cs)
    print color_p + 'Subprocess: Writing into foxbms.elf\n' + color_c + cmd + color_n + '\n'
    print 'gdb output:\n-----------'
    p = subprocess.Popen(cmd, shell=True)
    p.wait()

    # write checksum into foxbms_flashheader.bin file
    tool = 'arm-none-eabi-objcopy -v'
    binFlashHeaderPath = os.path.join(*['build', 'src', 'general', 'foxbms_flashheader.bin'])
    cmd = ' '.join([tool, '-j', '.flashheader', '-O', 'binary', elfFilePath, binFlashHeaderPath])
    print '\n' + color_p + 'Subprocess: Writing into foxbms_flashheader.bin\n' + color_c + cmd + color_n + '\n'
    print 'objcopy output:\n---------------'
    p = subprocess.Popen(cmd, shell=True)
    p.wait()


def flash(bld):
    subprocess.call("python tools/flashtool/stm32_loader.py -p COM10 -e -w -v -a 0x08000000 " + bld.path.abspath() + ("/build/src/general/foxbms_flash.bin"), shell=True)
    subprocess.call("python tools/flashtool/stm32_loader.py -p COM10 -w -v -a 0x080FFF00 " + bld.path.abspath() + ("/build/src/general/foxbms_flashheader.bin"), shell=True)


def cpplint(bld):
    tool = 'python'
    configFile = os.path.join(*['tools', 'cpplint', 'checkall.py'])
    cmd = ' '.join([tool, configFile])
    p = subprocess.Popen(cmd, shell=True)
    p.wait()

def doxygen(bld):
    if bld.env.DOXYGEN:
        _docbuilddir = os.path.join(bld.bldnode.abspath(), 'doc')
        if not os.path.exists(_docbuilddir):
            os.makedirs(_docbuilddir)
        doxyConfFile = os.path.join(*['doc', 'doxygen', 'doxygen.conf'])
        bld(features="doxygen", doxyfile=doxyConfFile)


def sphinx(bld):#
    '''Builds the sphinx documentation defined in doc/sphinx

    Args:
        bld

    Returns:
        -none-
    '''
    sphinxPath = os.path.join(*['doc', 'sphinx'])
    bld.recurse(sphinxPath)


def size(bld):
    '''Calculates the size of all libraries the foxbms.elf file.
    
    Gets all libraries in the build directory (by file extension *.o) and the main foxbms.elf binary and processes
    the libraries with size in berkley format.

    Args:
        bld

    Returns:
        -none-
    '''
    objlist = ''
    for root, dirs, files in os.walk("build"):
        for file in files:
            if file.endswith(".o"):
                objlist = objlist + " " + os.path.join(root, file)
    tool = 'arm-none-eabi-size'
    formatArg = '--format=berkeley'
    elfFilePath = os.path.join(*['build', 'src', 'general', 'foxbms.elf'])
    cmd = ' '.join([tool, formatArg, elfFilePath, objlist])
    print '\n' + color_p + 'Subprocess: elf size\n' + color_c + cmd + color_n + '\n'
    print 'size output:\n---------------'
    p = subprocess.Popen(cmd, shell=True)
    p.wait()


# Task generation: waf instructions for running arm-none-eabi-strip during release build
class strip(Task.Task):
    run_str = '${STRIP} ${SRC}'
    color = 'BLUE'
    # after   = ['cprogram', 'cxxprogram', 'cshlib', 'cxxshlib', 'fcprogram', 'fcshlib']

@TaskGen.feature('strip')
@TaskGen.after('apply_link')
def add_strip_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('strip', link_task.outputs[0])



# Task generation: converts .elf to .hex
class hexgen(Task.Task):
    run_str = '${hexgen} -O ihex ${SRC} src/general/foxbms.hex'
    color = 'CYAN'
    # after   = ['cprogram', 'cxxprogram', 'cshlib', 'cxxshlib', 'fcprogram', 'fcshlib']

@TaskGen.feature('hexgen')
@TaskGen.after('apply_link')
def add_hexgen_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('hexgen', link_task.outputs[0])



# Task generation: converts .elf to .bin
class binbkpramgen(Task.Task):
    run_str = '${hexgen} -j .bkp_ramsect -O binary ${SRC} src/general/foxbms_bkpram.bin'
    color = 'RED'
    # after   = ['cprogram', 'cxxprogram', 'cshlib', 'cxxshlib', 'fcprogram', 'fcshlib']
class binflashheadergen(Task.Task):
    run_str = '${hexgen} -j .flashheader -O binary ${SRC} src/general/foxbms_flashheader.bin'
    color = 'RED'
    # after   = ['cprogram', 'cxxprogram', 'cshlib', 'cxxshlib', 'fcprogram', 'fcshlib']
class binflashgen(Task.Task):
    run_str = '${hexgen} -R .bkp_ramsect -R .flashheader -O binary ${SRC} src/general/foxbms_flash.bin'
    color = 'RED'
    # after   = ['cprogram', 'cxxprogram', 'cshlib', 'cxxshlib', 'fcprogram', 'fcshlib']

@TaskGen.feature('bingen')
@TaskGen.after('apply_link')
def add_bingen_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('binbkpramgen', link_task.outputs[0])
    self.create_task('binflashgen', link_task.outputs[0])
    self.create_task('binflashheadergen', link_task.outputs[0])



# Task generation: waf instructions for startup script compile routine
import waflib.Tools.asm  # import before redefining
from waflib.TaskGen import extension
@extension('.S')
def asm_hook(self, node):
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



