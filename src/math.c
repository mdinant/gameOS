#include <system.h>

int abs(int number)
{
	if(number < 0)
		return (number * -1);
		
	return number;
}

float fabs(float number)
{
	if(number < 0.0f)
		return number * -1.0f;
		
	return number;
}

void hex_to_char(unsigned long number, char * ret_value)
{
	unsigned long quotient;
	char hex_value[8];
	int i, rem;
	

	quotient = number;
	if(number == 0)
	{
		ret_value[0] = '0';
		ret_value[1] = '\0';
		return;
	}	
	i=0;
	while (quotient>0)
	{
		rem = quotient % 16;
		quotient = quotient / 16;
		if(rem >9)
		hex_value[i] = (char)rem+55;
		else hex_value[i] = (char)rem+48;
		i++;
	}
	hex_value[i]='\0';
	// Reverse String
	int y = i-1;
	int z;
	for(z = 0; z < i; z++)
	{
		ret_value[z] = hex_value[y];
		y--;
	}
	ret_value[i] = '\0';

}
