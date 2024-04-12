 */
void get_token(FILE *pnm_file, char *token)
{
  int i = 0;
  int ret;
  /* remove white-space and comment lines */
  do
  {
    ret = fgetc(pnm_file);
    if (ret == '#') {
      /* the rest of this line is a comment */
      do
      {
        ret = fgetc(pnm_file);
      }
      while ((ret != '\n') && (ret != '\r') && (ret != EOF));
    }
    if (ret == EOF) break;
    token[i] = (unsigned char) ret;
  }
  while ((token[i] == '\n') || (token[i] == '\r') || (token[i] == ' '));
  /* read string */
  do
  {
    ret = fgetc(pnm_file);
    if (ret == EOF) break;
    i++;
    token[i] = (unsigned char) ret;
  }
  while ((token[i] != '\n') && (token[i] != '\r') && (token[i] != ' '));
  token[i] = '\0';
  return;
}
