#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void);

int
main()
{
	char *s0 = "0123456788";
	char *s1 = "0123456789";
	char *s2 = "01234567890123456789";
	char *s3 = "0123456788-0123456789";

	/*
	 * strncmp()'s return value is in accordance with whether the first
	 * string is greater than, equal to, or less than the second string
	 */

	printf("strcmp('%s', '%s') = %d\n", s0, s1, strcmp(s0, s1)); /* s0 < s1 */
	printf("strcmp('%s', '%s') = %d\n", s0, s0, strcmp(s0, s0)); /* s0 == s0 */
	printf("strcmp('%s', '%s') = %d\n", s1, s0, strcmp(s1, s0)); /* s1 > s0 */

	printf("strcmp('%s', '%s') = %d\n", s1, s2, strcmp(s1, s2));
	printf("strcmp('%s', '%s') = %d\n", s1, s3, strcmp(s1, s2));

	printf("strncmp('%s', '%s', %d) = %d\n", s1, s2, (int) strlen(s1), strncmp(s1, s2, strlen(s1)));
	printf("strncmp('%s', '%s', %d) = %d\n", s1, s2, (int) strlen(s2), strncmp(s1, s2, strlen(s2)));
	printf("strncmp('%s', '%s', %d) = %d\n", s1, s3, (int) strlen(s1), strncmp(s1, s3, strlen(s1)));
	printf("strncmp('%s', '%s', %d) = %d\n", s1, s3, (int) strlen(s3), strncmp(s1, s3, strlen(s2)));

	s0 = "010029";
	s1 = "010030";

	printf("strcmp('%s', '%s') = %d\n", s0, s1, strcmp(s0, s1)); /* s0 < s1 */
	printf("strcmp('%s', '%s') = %d\n", s0, s0, strcmp(s0, s0)); /* s0 == s0 */
	printf("strcmp('%s', '%s') = %d\n", s1, s0, strcmp(s1, s0)); /* s1 > s0 */

	s0 = "010030-rc2";
	s1 = "010030";

	printf("strcmp('%s', '%s') = %d\n", s0, s1, strcmp(s0, s1)); /* s0 < s1 */
	printf("strcmp('%s', '%s') = %d\n", s0, s0, strcmp(s0, s0)); /* s0 == s0 */
	printf("strcmp('%s', '%s') = %d\n", s1, s0, strcmp(s1, s0)); /* s1 > s0 */

	exit(0);
}
