char *
superstrdup(const char *str)
{
	size_t len = strlen(str);
	char *answer = malloc(len + 1);

	if (answer)
		memcpy(answer, str, len + 1); /* strcpy(answer, str); */

	return answer;
}
