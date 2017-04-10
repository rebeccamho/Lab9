//#Calib.c
//Matt Owens and Rebecca Ho
//April 9, 2017

uint16_t const ADCdata[53]={0,382,436,492,548,606,665,724,785,847,911,
	975,1041,1108,1176,1245,1316,1388,1461,1536,1612,
	1689,1768,1848,1930,2013,2098,2184,2272,2361,2452,
	2544,2639,2734,2832,2931,3032,3135,3239,3345,3453,
	3563,3675,3788,3903,4021,4095,4095,4095,4095,4095,4095,4096};

uint16_t const Tdata[53]={4000,4000,3940,3880,3820,3760,3700,3640,3580,3520,3460,
     3400,3340,3280,3220,3160,3100,3040,2980,2920,2860,
     2800,2740,2680,2620,2560,2500,2440,2380,2320,2260,
     2200,2140,2080,2020,1960,1900,1840,1780,1720,1660,
     1600,1540,1480,1420,1360,1300,1240,1180,1120,1060,1000,1000};

bool dataIsWindowBoundary = false;
uint16_t findLowIndex(uint32_t adc_val){
	dataIsWindowBoundary = false;
	//data is first in array
	if(adc_val == ADCdata[0]){
		dataIsWindowBoundary = true;
		return 0;
	}
	
	int i;
	for(i = 1; i < 53; i++){
		//window found
		if(adc_val < ADCdata[i] && adc_val > ADCdata[i-1]){
			return i-1;
		}
		//data is exact window boundary
		else if(adc_val == ADCdata[i]){
			dataIsWindowBoundary = true;
			return i;
		}
	}
	return -1;
}

//temp_low*1000 - (temp_low - temp_high)*1000/(adc_high - adc_low) * (adc_current-adc_low)
uint32_t ConvertADCToTemp(uint32_t adc_value){
	int index = findLowIndex(adc_value);
	if(dataIsWindowBoundary)
		return Tdata[index];
	
	int temp_low = Tdata[index];
	int temp_high = Tdata[index + 1];
	int adc_low = ADCdata[index];
	int adc_high = ADCdata[index+1];
	
	return temp_low * 1000 - (((temp_low - temp_high) * 10000) / (adc_high - adc_low) * (adc_value - adc_low));
}