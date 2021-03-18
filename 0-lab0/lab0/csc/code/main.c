#include <stdio.h>
#include <fibo.h>
int main()
{
	int n,i;
	scanf("%d",&n);
	for(i=1;i<=n;i++){
		printf("%d ",fibo(i));
	}
	return 0;
}
