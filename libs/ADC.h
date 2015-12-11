#ifndef ADC_H_
#define ADC_H_

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog to digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables autotriggering.Since we are
	// in Free Running Mode, a new conversion will trigger whenever
	// the previous conversion completes.
}
void ADC_Select(unsigned char adcMux) {
	switch(adcMux)
	{
		case 0: 
			ADMUX = (ADMUX & 0xF8);
			break;
		case 1:
			ADMUX = (ADMUX & 0xF8) | (1 << MUX0);
			break;
		case 2:
			ADMUX = (ADMUX & 0xF8) | (1 << MUX1);
			break;
		case 3:
			ADMUX = (ADMUX & 0xF8) | (1 << MUX1) |  (1 << MUX0);
			break;
		case 4:
			ADMUX = (ADMUX & 0xF8) | (1 << MUX2);
			break;
		case 5:
			ADMUX = (ADMUX & 0xF8) | (1 << MUX2) | (1 << MUX0);
			break;
		case 6:
			ADMUX = (ADMUX & 0xF8) | (1 << MUX2) | (1 << MUX1);
			break;
		case 7:
			ADMUX = (ADMUX & 0xF8) | (1 << MUX2) | (1 << MUX1) |  (1 << MUX0);
			break;
	}
}

#endif // ADC_H_