#include<stdio.h>
int main(){
	int n;
	scanf("%d", &n);

	int len = 0;
	int digits[10];
	while (n > 0) {
		digits[len] = n % 10;
		n = n / 10;
		len ++;
	}
	int i = 0;
	int isPali = 1; // true default

	for (i = 0; i < len/2; i++) {
		// printf("%d", digits[i]);
		if (digits[i] == digits[len - i -1]) {
			// pass
		} else {
			isPali = 0;
			break;
		}
	}
	if (isPali == 1) {
		printf("is Pali");
	} else {
		printf("not Pali");
	}
	return 0;

}
