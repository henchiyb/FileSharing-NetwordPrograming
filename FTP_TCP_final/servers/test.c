#include <dirent.h>
#include <stdio.h>

int main(void)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir(".");
  if (d)
  {
    char* path;
    asprintf(&path,"%s/n", username, params);
    while ((dir = readdir(d)) != NULL)
    {
      printf("%s\n", dir->d_name);
    }

    closedir(d);
  }

  return(0);
}
