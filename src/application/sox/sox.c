/**
 *
 * @copyright &copy; 2010 - 2016, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
 *
 * BSD 3-Clause License
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * We kindly request you to use one or more of the following phrases to refer to foxBMS in your hardware, software, documentation or advertising materials:
 *
 * &Prime;This product uses parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product includes parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product is derived from foxBMS&reg;&Prime;
 *
 */

/**
 * @file    sox.c
 * @author  foxBMS Team
 * @date    13.11.2015 (date of creation)
 * @ingroup APPLICATION
 * @prefix  SOX
 *
 * @brief   SOX module, responsible for calculation of current derating and of SOC
 */



/*================== Includes =============================================*/
#include "general.h"
#include "sox.h"
#include "eepr.h"
#include "mcu.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

static DATA_BLOCK_CURRENT_s current_tab;
static DATA_BLOCK_CELLVOLTAGE_s cellvoltage;
static DATA_BLOCK_CELLTEMPERATURE_s celltemperature;
static DATA_BLOCK_MINMAX_s cellminmax;
static DATA_BLOCK_SOX_s sox;
static DATA_BLOCK_CANERRORSIG_s canerrtab;

static SOX_SOF_s values_sof;
static uint32_t soc_previous_current_timestamp = 0;

/** @{
 * module-local static Variables that are calculated at startup and used later to avoid divisions at runtime
 */
static float Slope_TLowDischa = 0.0;
static float Offset_TLowDischa = 0.0;
static float Slope_THighDischa = 0.0;
static float Offset_THighDischa = 0.0;

static float Slope_TLowCharge = 0.0;
static float Offset_TLowCharge = 0.0;
static float Slope_THighCharge = 0.0;
static float Offset_THighCharge = 0.0;

static float Slope_SocDischa = 0.0;
static float Offset_SocDischa = 0.0;
static float Slope_SocCharge = 0.0;
static float Offset_SocCharge = 0.0;

static float Slope_VoltageDischa = 0.0;
static float Offset_VoltageDischa = 0.0;
static float Slope_VoltageCharge = 0.0;
static float Offset_VoltageCharge = 0.0;
/** @} */

/*================== Function Prototypes ==================================*/
static float SOC_GetFromVoltage(float voltage);
static void SOF_Calculate(int16_t maxtemp, int16_t mintemp, uint16_t maxvolt, uint16_t minvolt, uint16_t maxsoc, uint16_t minsoc);
static void SOF_CalculateVoltageBased (float MinVoltage,float MaxVoltage, SOX_SOF_s *ResultValues);
static void SOF_CalculateSocBased (float MinSoc,float MaxSoc, SOX_SOF_s *ResultValues);
static void SOF_CalculateTemperatureBased (float MinTemp,float MaxTemp, SOX_SOF_s *ResultValues);
static void SOF_MinimumOfThreeSofValues(SOX_SOF_s Ubased, SOX_SOF_s Sbased, SOX_SOF_s Tbased, SOX_SOF_s *resultValues);
static float SOF_MinimumOfThreeValues (float value1,float value2, float value3);

/*================== Function Implementations =============================*/

void SOC_Init(void) {
    SOX_SOC_s soc = {50.0, 50.0, 50.0};

    DATA_GetTable(&current_tab, DATA_BLOCK_ID_CURRENT);
    soc_previous_current_timestamp = current_tab.timestamp;
    soc = EEPR_Get_nvsoc();
    sox.soc_mean = soc.mean;
    sox.soc_min = soc.min;
    sox.soc_max = soc.max;
    //Alternatively, SOC can be initialized with {V,SOC} lookup table if available
    //with the function SOC_Init_Lookup_Table()
    sox.state = 0;
    sox.timestamp = 0;
    sox.previous_timestamp = 0;
    DATA_StoreDataBlock(&sox, DATA_BLOCK_ID_SOX);
}

void SOC_SetValue(float soc_value) {
    SOX_SOC_s soc = {50.0, 50.0, 50.0};

    if (soc_value < 0.0) {
        soc_value = 0.0;
    }
    if (soc_value > 100.0) {
        soc_value = 100.0;
    }

    soc.mean = soc_value;
    soc.min = soc_value;
    soc.max = soc_value;
    EEPR_Set_nvsoc(&soc);

    sox.soc_mean = soc.mean;
    sox.soc_min = soc.min;
    sox.soc_max = soc.max;
    sox.state = 0;
    sox.timestamp = 0;
    sox.previous_timestamp = 0;
    int interruptstate = MCU_DisableINT();          // FIXME do not block while accessing database:
    DATA_StoreDataBlock(&sox,DATA_BLOCK_ID_SOX);    // do not use module data buffer (sox) in different tasks when
                                                    // accessing database
    MCU_RestoreINT(interruptstate);
}


void SOC_Init_Lookup_Table(void) {
    float soc_min = 50.0;
    float soc_max = 50.0;
    float soc_mean = 50.0;

    float current = 50.0;
    float delta_min = 50.0;
    float delta_max = 50.0;

    SOX_SOC_s soc = {50.0, 50.0, 50.0};

    DATA_GetTable(&cellminmax, DATA_BLOCK_ID_MINMAX);
    DATA_GetTable(&current_tab, DATA_BLOCK_ID_CURRENT);

    soc_mean = SOC_GetFromVoltage((float)(cellminmax.voltage_mean));
    soc_min = SOC_GetFromVoltage((float)(cellminmax.voltage_min));
    soc_max = SOC_GetFromVoltage((float)(cellminmax.voltage_max));

    soc_previous_current_timestamp = current_tab.timestamp;

    if (current_tab.current >= 0.0) {
        current = current_tab.current;
    }
    else {
        current = -current_tab.current;
    }

    if (cellminmax.voltage_min>=cellminmax.previous_voltage_min) {
        delta_min = cellminmax.voltage_min-cellminmax.previous_voltage_min;
    }
    else {
        delta_min = cellminmax.previous_voltage_min-cellminmax.voltage_min;
    }

    if (cellminmax.voltage_max>=cellminmax.previous_voltage_max) {
        delta_max = cellminmax.voltage_max-cellminmax.previous_voltage_max;
    }
    else {
        delta_max = cellminmax.previous_voltage_max-cellminmax.voltage_max;
    }
    //no current and voltages relaxed: SOC can be initialized via look-up table
    if ((current < SOX_SOC_INIT_CURRENT_LIMIT) && (delta_min < SOX_DELTA_MIN_LIMIT) && (delta_max < SOX_DELTA_MAX_LIMIT)) {
        soc.mean = soc_mean;
        soc.min = soc_min;
        soc.max = soc_max;
        EEPR_Set_nvsoc(&soc);

        sox.soc_mean = soc_mean;
        sox.soc_min = soc_min;
        sox.soc_max = soc_max;
        sox.state = 0;
        sox.timestamp = 0;
        sox.previous_timestamp = 0;
        DATA_StoreDataBlock(&sox, DATA_BLOCK_ID_SOX);

    }
    else {
        soc = EEPR_Get_nvsoc();
        sox.soc_mean = soc.mean;
        sox.soc_min = soc.min;
        sox.soc_max = soc.max;
        sox.state = 0;
        sox.timestamp = 0;
        sox.previous_timestamp = 0;
        DATA_StoreDataBlock(&sox,DATA_BLOCK_ID_SOX);
    }
}


void SOC_Ctrl(void) {
    uint32_t timestamp = 0;
    uint32_t previous_timestamp = 0;
    uint32_t timestep = 0;

    SOX_SOC_s soc = {50.0, 50.0, 50.0};
    float deltaSOC = 0.0;

    DATA_GetTable(&current_tab, DATA_BLOCK_ID_CURRENT);

    timestamp = current_tab.timestamp;
    previous_timestamp = current_tab.previous_timestamp;

    if (soc_previous_current_timestamp != timestamp) { // check if current measurement has been updated
        timestep = timestamp - previous_timestamp;
        if (timestep > 0) {

            soc = EEPR_Get_nvsoc();
            // Current in charge direction negative means SOC increasing --> BAT naming, not ROB
            //soc_mean = soc_mean - (current_tab.current/*mA*/ /(float)SOX_CELL_CAPACITY /*mAh*/) * (float)(timestep) * (10.0/3600.0); /*milliseconds*/
            deltaSOC = (((current_tab.current)*(float)(timestep)/10))/(3600.0*SOX_CELL_CAPACITY); // ((mA *ms *(1s/1000ms)) / (3600(s/h) *mAh)) *100%
            soc.mean = soc.mean - deltaSOC;
            soc.min = soc.min - deltaSOC;
            soc.max = soc.max - deltaSOC;
            if (soc.mean > 100.0) { soc.mean = 100.0; }
            if (soc.mean < 0.0)   { soc.mean = 0.0;   }
            if (soc.min > 100.0)  { soc.min = 100.0;  }
            if (soc.min < 0.0)    { soc.min = 0.0;    }
            if (soc.max > 100.0)  { soc.max = 100.0;  }
            if (soc.max < 0.0)    { soc.max = 0.0;    }

            EEPR_Set_nvsoc(&soc);

            sox.state++;
            sox.soc_mean = soc.mean;
            sox.soc_min = soc.min;
            sox.soc_max = soc.max;
            sox.previous_timestamp = previous_timestamp;
            sox.timestamp = timestamp;  // soc timestamp is current(I) timestamp
            DATA_StoreDataBlock(&sox, DATA_BLOCK_ID_SOX);
        }
    } // end check if current measurement has been updated
    //update the variable for the next check
    soc_previous_current_timestamp = current_tab.timestamp;
}

void SOF_Init(void) {
    Slope_TLowDischa = (sox_sof_config.I_DischaMax_Cont - sox_sof_config.I_Limphome) / (sox_sof_config.Cutoff_TLow_Discha - sox_sof_config.Limit_TLow_Discha);
    Offset_TLowDischa = sox_sof_config.I_Limphome - (Slope_TLowDischa * sox_sof_config.Limit_TLow_Discha);
    Slope_THighDischa = (0 - sox_sof_config.I_DischaMax_Cont) / (sox_sof_config.Limit_THigh_Discha - sox_sof_config.Cutoff_THigh_Discha);
    Offset_THighDischa = 0 - Slope_THighDischa * sox_sof_config.Limit_THigh_Discha;

    Slope_TLowCharge =  (sox_sof_config.I_ChargeMax_Cont - 0) / (sox_sof_config.Cutoff_TLow_Charge - sox_sof_config.Limit_TLow_Charge);
    Offset_TLowCharge = 0 - (Slope_TLowCharge * sox_sof_config.Limit_TLow_Charge);
    Slope_THighCharge = (0 - sox_sof_config.I_ChargeMax_Cont) / (sox_sof_config.Limit_THigh_Charge - sox_sof_config.Cutoff_THigh_Charge);
    Offset_THighCharge = 0 - Slope_THighCharge * sox_sof_config.Limit_THigh_Charge;

    Slope_SocDischa = (sox_sof_config.I_DischaMax_Cont - sox_sof_config.I_Limphome) / (sox_sof_config.Cutoff_Soc_Discha - sox_sof_config.Limit_Soc_Discha);
    Offset_SocDischa = sox_sof_config.I_Limphome - (Slope_SocDischa * sox_sof_config.Limit_Soc_Discha);

    Slope_SocCharge = (sox_sof_config.I_ChargeMax_Cont - 0.0) / (sox_sof_config.Cutoff_Soc_Charge - sox_sof_config.Limit_Soc_Charge);
    Offset_SocCharge = 0 - Slope_SocCharge * sox_sof_config.Limit_Soc_Charge;

    Slope_VoltageDischa = (sox_sof_config.I_DischaMax_Cont - 0) / (sox_sof_config.Cutoff_Voltage_Discha - sox_sof_config.Limit_Voltage_Discha);
    Offset_VoltageDischa = 0 - Slope_SocDischa * sox_sof_config.Limit_Soc_Discha;

    Slope_VoltageCharge = (sox_sof_config.I_ChargeMax_Cont - 0) / (sox_sof_config.Cutoff_Voltage_Charge - sox_sof_config.Limit_Voltage_Charge);
    Offset_VoltageCharge = 0 - Slope_VoltageCharge * sox_sof_config.Limit_Soc_Discha;
}

void SOF_Ctrl(void) {
    DATA_GetTable(&cellvoltage, DATA_BLOCK_ID_CELLVOLTAGE);
    DATA_GetTable(&celltemperature, DATA_BLOCK_ID_CELLTEMPERATURE);
    DATA_GetTable(&cellminmax, DATA_BLOCK_ID_MINMAX);
    DATA_GetTable(&sox,DATA_BLOCK_ID_SOX);
    DATA_GetTable(&canerrtab, DATA_BLOCK_ID_CANERRORSIG);
    // if Contactor MainPlus and MainMinus are not closed (when they are closed, state_feedback is 0x0C)
    if (canerrtab.contactor_state_feedback != 0x0C){
        sox.sof_continuous_charge = 0.0;
        sox.sof_continuous_discharge = 0.0;
        sox.sof_peak_charge = 0.0;
        sox.sof_peak_discharge = 0.0;
    } else {
        SOF_Calculate(cellminmax.temperature_max, cellminmax.temperature_min, cellminmax.voltage_max, cellminmax.voltage_min, (uint16_t)(100.0*sox.soc_max), (uint16_t)(100.0*sox.soc_min));
        sox.sof_continuous_charge = values_sof.current_Charge_cont_max;
        sox.sof_continuous_discharge = values_sof.current_Discha_cont_max;
        sox.sof_peak_charge = values_sof.current_Charge_peak_max;
        sox.sof_peak_discharge = values_sof.current_Discha_peak_max;
    }
    DATA_StoreDataBlock(&sox, DATA_BLOCK_ID_SOX);
}

/**
 * @brief   look-up table for SOC initialization (mean, min and max).
 *
 * @param   voltage: voltage of battery cell
 *
 * @return  SOC value
 */

static float SOC_GetFromVoltage(float voltage) {

    float SOC = 0;

    SOC = 50.0;

    return SOC;
}


/**
 * @brief   calculates State of function which means how much current can be delivered by battery to stay in safe operating area.
 *
 * @param   maxtemp       maximum temperature in system in °C
 * @param   mintemp       minimum temperature in system in °C
 * @param   maxvolt       maximum voltage in system in mV
 * @param   minvolt       minimum voltage in system in mV
 * @param   maxsoc        maximum soc in system with resolution 0.01% (0..10000)
 * @param   minsoc        minimum soc in system with resolution 0.01% (0..10000)
 *
 * @return  void
 */
static void SOF_Calculate(int16_t maxtemp, int16_t mintemp, uint16_t maxvolt, uint16_t minvolt, uint16_t maxsoc, uint16_t minsoc) {
    static SOX_SOF_s UbasedSof = {0.0, 0.0, 0.0};
    static SOX_SOF_s SbasedSof = {0.0, 0.0, 0.0};
    static SOX_SOF_s TbasedSof = {0.0, 0.0, 0.0};
    SOF_CalculateVoltageBased((float)minvolt,(float)maxvolt,&UbasedSof );
    SOF_CalculateSocBased((float)minsoc,(float)maxsoc,&SbasedSof);
    SOF_CalculateTemperatureBased((float)mintemp,(float)maxtemp,&TbasedSof);
    SOF_MinimumOfThreeSofValues(UbasedSof, SbasedSof, TbasedSof, &values_sof);
}

/**
 *  @brief  calculates the SoF from voltage data (i.e., minimum and maximum voltage)
 *
 *  @param  MinVoltage minimum sell voltage
 *  @param  MaxVoltage maximum cell voltage
 *  @param  ResultValues Voltage-based SOF
 *
 *  @return void
 */
static void SOF_CalculateVoltageBased (float MinVoltage,float MaxVoltage, SOX_SOF_s *ResultValues) {
    // min voltage issues
    if (MinVoltage <= sox_sof_config.Limit_Voltage_Discha) {
        ResultValues->current_Discha_cont_max = 0;
        ResultValues->current_Discha_peak_max = 0;
    }
    else {
        if (MinVoltage <= sox_sof_config.Cutoff_Voltage_Discha) {
            ResultValues->current_Discha_cont_max = (Slope_VoltageDischa * (MinVoltage-sox_sof_config.Limit_Voltage_Discha));
            ResultValues->current_Discha_peak_max = ResultValues->current_Discha_cont_max;
        }
        else {
            ResultValues->current_Discha_cont_max = sox_sof_config.I_DischaMax_Cont;
            ResultValues->current_Discha_peak_max = sox_sof_config.I_DischaMax_Cont;
        }
    }
    // max voltage issues
    if (MaxVoltage >= sox_sof_config.Limit_Voltage_Charge) {
        ResultValues->current_Charge_cont_max = 0;
        ResultValues->current_Charge_peak_max = 0;
    }
    else {
        if (MaxVoltage >= sox_sof_config.Cutoff_Voltage_Charge) {
            ResultValues->current_Charge_cont_max =  (Slope_VoltageCharge * (MaxVoltage- sox_sof_config.Limit_Voltage_Charge));
            ResultValues->current_Charge_peak_max = ResultValues->current_Charge_cont_max;
        }
        else {
            ResultValues->current_Charge_cont_max = sox_sof_config.I_ChargeMax_Cont;
            ResultValues->current_Charge_peak_max = sox_sof_config.I_ChargeMax_Cont;
        }
    }
}

/**
 * @brief   calculates the SoF from SoC data (i.e., minimum and maximum SoC of cells, both values are equal in case of Ah-counter)
 *
 * @param   MinSoc minimum State of Charge
 * @param   MaxSoc maximum State of Charge
 * @param   ResultValues pointer where to store the results
 *
 * @return  void
 */
static void SOF_CalculateSocBased (float MinSoc,float MaxSoc, SOX_SOF_s *ResultValues) {
    // min voltage issues
    if (MinSoc <= sox_sof_config.Limit_Soc_Discha){
        ResultValues->current_Discha_cont_max = sox_sof_config.I_Limphome;
        ResultValues->current_Discha_peak_max = sox_sof_config.I_Limphome;
    }
    else {
        if (MinSoc <= sox_sof_config.Cutoff_Soc_Discha){
            ResultValues->current_Discha_cont_max =  Slope_SocDischa * MinSoc + Offset_SocDischa;
            ResultValues->current_Discha_peak_max =  ResultValues->current_Discha_cont_max;
        }
        else{
            ResultValues->current_Discha_cont_max =  sox_sof_config.I_DischaMax_Cont;
            ResultValues->current_Discha_peak_max =  sox_sof_config.I_DischaMax_Cont;
        }
    }

    // max voltage issues
    if (MaxSoc >= sox_sof_config.Limit_Soc_Charge){
        ResultValues->current_Charge_cont_max = 0;
        ResultValues->current_Charge_peak_max = 0;
    }
    else {
        if (MaxSoc >= sox_sof_config.Cutoff_Soc_Charge){
            ResultValues->current_Charge_cont_max =  Slope_SocCharge * MaxSoc + Offset_SocCharge;
            ResultValues->current_Charge_peak_max = ResultValues->current_Charge_cont_max;
        }
        else{
            ResultValues->current_Charge_cont_max = sox_sof_config.I_ChargeMax_Cont;
            ResultValues->current_Charge_peak_max = sox_sof_config.I_ChargeMax_Cont;
        }
    }
}

/**
 * @brief   calculates the SoF from temperature data (i.e., minimum and maximum temperature of cells)
 *
 * @param   MinTemp minimum temperature of cells
 * @param   MaxTemp maximum temperature of cells
 * @param   ResultValues pointer where to store the results
 *
 * @return  void
 */
static void  SOF_CalculateTemperatureBased (float MinTemp,float MaxTemp, SOX_SOF_s *ResultValues) {

    SOX_SOF_s dummyResultValues = {0.0, 0.0, 0.0};
    // Temperature low Discharge
    if (MinTemp <= sox_sof_config.Limit_TLow_Discha) {
        ResultValues->current_Discha_cont_max = sox_sof_config.I_Limphome;
        ResultValues->current_Discha_peak_max = sox_sof_config.I_Limphome;
    }
    else {
        if (MinTemp <= sox_sof_config.Cutoff_TLow_Discha) {
            ResultValues->current_Discha_cont_max = Slope_TLowDischa * MinTemp + Offset_TLowDischa;
            ResultValues->current_Discha_peak_max = ResultValues->current_Discha_cont_max;
        }
        else {
            ResultValues->current_Discha_cont_max = sox_sof_config.I_DischaMax_Cont;
            ResultValues->current_Discha_peak_max = sox_sof_config.I_DischaMax_Cont;
        }
    }
    // Temperature low charge
    if (MinTemp <= sox_sof_config.Limit_TLow_Charge) {
        ResultValues->current_Charge_cont_max = 0;
        ResultValues->current_Charge_peak_max = 0;
    }
    else {
        if (MinTemp <= sox_sof_config.Cutoff_TLow_Charge) {
            ResultValues->current_Charge_cont_max = Slope_TLowCharge * MinTemp + Offset_TLowCharge;
            ResultValues->current_Charge_peak_max = ResultValues->current_Charge_cont_max;
        }
        else{
            ResultValues->current_Charge_cont_max =  sox_sof_config.I_ChargeMax_Cont;
            ResultValues->current_Charge_peak_max =  sox_sof_config.I_ChargeMax_Cont;
        }
    }
    // Temperature high discharge
    if (MaxTemp >= sox_sof_config.Limit_THigh_Discha) {
        ResultValues->current_Discha_cont_max = 0;
        ResultValues->current_Discha_peak_max = 0;
        dummyResultValues.current_Discha_cont_max = 0;
        dummyResultValues.current_Discha_peak_max = 0;
    }
    else {
        if (MaxTemp >= sox_sof_config.Cutoff_THigh_Discha) {
            dummyResultValues.current_Discha_cont_max = (Slope_THighDischa * MaxTemp + Offset_THighDischa);
            dummyResultValues.current_Discha_peak_max = dummyResultValues.current_Discha_cont_max;
        }
        else {
            // do nothing because this situation is handled with MinTemp
            dummyResultValues.current_Discha_cont_max = sox_sof_config.I_DischaMax_Cont;
            dummyResultValues.current_Discha_peak_max = sox_sof_config.I_DischaMax_Cont;

        }
        // take the smaller current as limit
        if (dummyResultValues.current_Discha_cont_max<ResultValues->current_Discha_cont_max)
            ResultValues->current_Discha_cont_max = dummyResultValues.current_Discha_cont_max;
        if (dummyResultValues.current_Discha_peak_max<ResultValues->current_Discha_peak_max)
            ResultValues->current_Discha_peak_max = dummyResultValues.current_Discha_peak_max;
    }
    // Temperature high Charge
    if (MaxTemp >= sox_sof_config.Limit_THigh_Charge) {
        ResultValues->current_Charge_cont_max = 0;
        ResultValues->current_Charge_peak_max = 0;
        dummyResultValues.current_Charge_cont_max = 0;
        dummyResultValues.current_Charge_peak_max = 0;
    }
    else {
        if (MaxTemp >= sox_sof_config.Cutoff_THigh_Charge){
            dummyResultValues.current_Charge_cont_max = (Slope_THighCharge * MaxTemp + Offset_THighCharge);
            dummyResultValues.current_Charge_peak_max = dummyResultValues.current_Charge_cont_max;
        }
        else {
            // do nothing because this situation is handled with MinTemp
            dummyResultValues.current_Charge_cont_max = sox_sof_config.I_ChargeMax_Cont;
            dummyResultValues.current_Charge_peak_max = sox_sof_config.I_ChargeMax_Cont;
        }
        // take the smaller current as limit
        if (dummyResultValues.current_Charge_cont_max<ResultValues->current_Charge_cont_max)
            ResultValues->current_Charge_cont_max = dummyResultValues.current_Charge_cont_max;
        if (dummyResultValues.current_Charge_peak_max<ResultValues->current_Charge_peak_max)
            ResultValues->current_Charge_peak_max = dummyResultValues.current_Charge_peak_max;
    }
}

/**
 * @brief   get the minimum current values of all variants of SoF calculation
 *
 * @param   Ubased voltage constrained current derating values
 * @param   Sbased SOC constrained current derating values
 * @param   Tbased temperature constrained current derating values
 * @param   resultValues minimum SoF current values
 *
 * @return  void
 */
static void SOF_MinimumOfThreeSofValues(SOX_SOF_s Ubased, SOX_SOF_s Sbased, SOX_SOF_s Tbased, SOX_SOF_s *resultValues) {
    resultValues->current_Charge_cont_max = SOF_MinimumOfThreeValues(Ubased.current_Charge_cont_max, Tbased.current_Charge_cont_max, Sbased.current_Charge_cont_max);
    resultValues->current_Charge_peak_max = SOF_MinimumOfThreeValues(Ubased.current_Charge_peak_max, Tbased.current_Charge_peak_max, Sbased.current_Charge_peak_max);
    resultValues->current_Discha_cont_max = SOF_MinimumOfThreeValues(Ubased.current_Discha_cont_max, Tbased.current_Discha_cont_max, Sbased.current_Discha_cont_max);
    resultValues->current_Discha_peak_max = SOF_MinimumOfThreeValues(Ubased.current_Discha_peak_max, Tbased.current_Discha_peak_max, Sbased.current_Discha_peak_max);
}

/**
 * @brief   calculates minimum of three values
 *
 * @param   value1
 * @param   value2
 * @param   value3
 *
 * @return minimum of the 3 parameters
 */
static float SOF_MinimumOfThreeValues (float value1,float value2, float value3) {
    float result = 0.0;
    if (value1 <= value2) {
        if (value3 <= value1){
            result = value3;
        }
        else {
            result = value1;
        }
    }
    else {
        if (value3 <= value2){
            result = value3;
        }
        else{
            result = value2;
        }
    }
    return result;
}

