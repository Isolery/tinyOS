
void delay(unsigned int i)
{
	while(i--);
}

int flag;

int main(void)
{
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
	}
	
	return 0;
}
