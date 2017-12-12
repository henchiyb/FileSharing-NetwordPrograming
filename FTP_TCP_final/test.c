#include <my_global.h>
#include <mysql.h>

void showErrorMessage(MYSQL* con){
   fprintf(stderr, "%s\n", mysql_error(con));
   exit(1);
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) 
  	showErrorMessage(con);

  if (mysql_real_connect(con, "localhost", "root", "ngoan123", 
          "file_share", 0, NULL, 0) == NULL) 
	showErrorMessage(con);
  if (mysql_query(con, "INSERT INTO users(username, password) values ('ha', 'ha')")) 
	showErrorMessage(con);
  if (mysql_query(con, "INSERT INTO users(username, password) values ('hop', 'hop')")) 
	showErrorMessage(con);
  mysql_query(con, "SELECT * FROM users(username, password) values ('hop', 'hop')"); 
  MYSQL_RES *result = mysql_store_result(con);
 
  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  MYSQL_FIELD *field;
  
  while ((row = mysql_fetch_row(result))) 
  { 
      for(int i = 0; i < num_fields; i++) 
      { 
          printf("%s ", row[i] ? row[i] : "NULL"); 
      } 
          printf("\n"); 
  }
  
  mysql_free_result(result);
  mysql_close(con);
  exit(0);
}


