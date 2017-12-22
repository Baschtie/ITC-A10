//Benötigte Libaries
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

//Funktionen
void clear_input(); //Löscht ungewollte zeicheneingaben / löscht den restlichen Input
int display_menu(); //Zeigt das Hauptmenu an und erwartet eine Eingabe und gibt diese zurück
void make_line(int length); //Erstellt eine Linie in der Konsole zur verbesserung der Übersicht
void display_calls(); //zeigt alle Einträge an
int get_last_id(); //Gibt die ID des letzten Eintrags an um fortlaufend zu zählen
void delete_call(int id); //Löscht einen Eintrag
void delete_month(); //Löscht alle Eintrage eines Monats
void display_users(); //Zeigt eine Liste mit benutzern und deren ID
void display_bill(float costs[24][3]); //Zeigt die Monatsrechnung für den gegebenen Monat an
char * get_user(int check_id); //Gibt den Namen eines Benutzers zurück
int get_user_count(); //Gibt die Anzahl der Benutzer zurück
float get_cost(char type[50], int duration_hour, int duration_minute, int duration_second, int beginn_hour, float costs[24][3]); //Erechnet die Kosten für ein Gespräch
int is_correct_date(int day, int month, int year); //Überprüft ob ein Datum korrekt ist
int is_correct_time(int hour, int minute, int second); //Überprüft ob eine Zeit korrekt ist
void new_call(); //Funktion um einen Anruf hinzuzufügen

struct User {
  char name[20];
  int id;
};

struct Date {
  int day;
  int month;
  int year;
};

struct Time {
  int hour;
  int minute;
  int second;
};

struct call {
  int id;
  struct Date beginn_date;
  struct Time beginn_time;
  struct Time duration;
  char destination[50];
  int caller_id;
};


int main() {

  //Erstelle User
  FILE *user_file;
  user_file = fopen("users.inp", "r");

  //Überprüfe existens der Users Datei
  if (!user_file) {
    fclose(user_file);
    printf("[Fehler] users.inp existiert nicht, Datei wird erstellt und benötigt noch Benutzer!\n");
    FILE *file;
    file = fopen("users.inp", "w");
    fclose(file);
    return 0;
  }

  //Erstelle User struct
  struct User Users[4];
  int user_counter;
  for (user_counter = 0; user_counter < 5; ++user_counter) {
    fscanf(user_file, "%s", Users[user_counter].name);
    Users[user_counter].id = user_counter;
  }
  fclose(user_file);

  //Erstelle Kosten
  FILE *cost_file;
  cost_file = fopen("costs.inp", "r");

  //Überprüfe existens der Users Datei
  if (!cost_file) {
    fclose(cost_file);
    printf("[Fehler] costs.inp existiert nicht, Datei wird erstellt und benötigt noch Kosten!\n");
    FILE *file;
    file = fopen("costs.inp", "w");
    fclose(file);
    return 0;
  }

  //Lade Kosten in Doppelfeld
  float costs[24][3];
  int i,j;
  for (i = 0; i < 24; ++i) {
    for (j = 0; j < 3; ++j) {
      fscanf(cost_file, "%f", &costs[i][j]);
    }
  }
  fclose(cost_file);

  //Datenbank
  FILE *data_file;
  data_file = fopen("database.dat", "r");

  //Überprüfe existens der DB Datei
  if (!data_file) {
    fclose(data_file);
    FILE *file;
    file = fopen("database.dat", "w");
    fclose(file);
    data_file = fopen("database.dat", "r");
  }
  fclose(data_file);

  while(1) {
    switch(display_menu()) {
      case 1: {
        new_call();
        break;}
      case 2:
        display_calls();
        make_line(49);
        break;
      case 3:
        display_calls();
        printf("Lösche Gespräch mit der ID:\n");
        int id;
        scanf("%i", &id);
        clear_input();
        delete_call(id);
        make_line(30);
        break;
      case 4:
        system("clear");
        delete_month();
        break;
      case 5:
        system("clear");
        display_bill(costs);
        break;
      case 6:
        printf("Beende Programm\n");
        make_line(30);
        return 1;
      default:
        printf("[FEHLER] Unbekannter Modus!\n");
        make_line(30);
    }

  }


  return 0;
}

void clear_input() {
  char  z;
  do  {
    z  =  getchar();
  }
  while(  z  !=  10  );
}

int display_menu() {
  printf("Telefonkostenverwaltung der IT Straße 96\n");
  printf("Wähle einen Modus:\n(1) Gespräch eingeben\n(2) Gespräche anzeigen\n(3) Gespräch löschen\n(4) Monatsgespräche löschen\n(5) Monatsrechnung\n(6) Programm beenden\n");
  make_line(30);

  int modus;
  scanf("%i", &modus);
  clear_input();
  system("clear");

  return modus;
}

void make_line(int length) {
  int i;
  for(i = 1; i < length; ++i) {
    printf("=");
  }
  printf("\n");
}

void display_calls() {
  FILE *data_file;
  data_file = fopen("database.dat", "r");

  struct call calls;
  printf(" %-3s | %-10s | %-7s | %-8s | %-10s\n", "ID", "Name", "Art", "Dauer", "Beginn");
  while(fread(&calls, sizeof(struct call), 1, data_file)) {

    char name[20];
    strcpy(name,get_user(calls.caller_id));

    printf(" %-3i |", calls.id);
    printf(" %-10s |", name);
    printf(" %-7s |", calls.destination);
    printf(" %02d:%02d:%02d |", calls.duration.hour, calls.duration.minute, calls.duration.second);
    printf(" %02d.%02d.%04d %02d:%02d\n", calls.beginn_date.day, calls.beginn_date.month, calls.beginn_date.year, calls.beginn_time.hour, calls.beginn_time.minute);
  }


  fclose(data_file);
}

void display_bill(float costs[24][3]) {

  int month,year;

  while(1) {

    make_line(30);
    printf("Welchger Monat soll Berechnet werden? (MM/YYYY)\n");
    scanf("%d/%d", &month, &year);
    clear_input();

    if(is_correct_date(01, month, year)) {
      break;
    } else {
      system("clear");
      printf("[FEHLER] Ungültige Eingabe!\n");
    }

  }

  system("clear");

  float total_cost = 0.0;

  int i;
  for (i = 1; i < get_user_count() + 1; ++i) {

    float user_cost = 0.0;

    FILE *data_file;
    data_file = fopen("database.dat", "r");

    printf("\n\n\nBenutzer: %s\n", get_user(i));

    struct call calls;
    printf(" %-8s | %-8s | %-16s | %-6s\n", "Art", "Dauer", "Beginn", "Kosten");
    while(fread(&calls, sizeof(struct call), 1, data_file)) {
      if((calls.caller_id == i) && (calls.beginn_date.year == year) && (calls.beginn_date.month == month)) {

        float current_cost = get_cost(calls.destination, calls.duration.hour, calls.duration.minute, calls.duration.second, calls.beginn_time.hour, costs);
        user_cost = user_cost + current_cost;

        char name[20];
        strcpy(name,get_user(calls.caller_id));

        printf(" %-8s |", calls.destination);
        printf(" %02d:%02d:%02d |", calls.duration.hour, calls.duration.minute, calls.duration.second);
        printf(" %02d.%02d.%04d %02d:%02d |", calls.beginn_date.day, calls.beginn_date.month, calls.beginn_date.year, calls.beginn_time.hour, calls.beginn_time.minute);
        printf(" %-4.2f €\n", current_cost);
      }
    }
    printf("\n%38s %-4.2f €\n", "Gesamt: ", user_cost);
    total_cost = total_cost + user_cost;
    fclose(data_file);
  }

  make_line(47);
  printf("%38s %-4.2f €\n", "GESAMT: ", total_cost);
  make_line(47);
  printf("\n\n");

}

void display_users() {
  FILE *user_file;
  user_file = fopen("users.inp", "r");
  printf(" %-3s | %-20s\n", "ID", "Name");

  int id = 0;
  char name[20];

  while(fscanf(user_file, "%s", name) != EOF) {
    ++id;
    printf(" %-3i | %-20s\n", id, name);
  }
  
  fclose(user_file);
}

char * get_user(int check_id) {
  FILE *user_file;
  user_file = fopen("users.inp", "r");

  int id = 0;
  static char result[10];

  while(fscanf(user_file, "%s", result) != EOF) {
    ++id;
    if (id == check_id) {
      fclose(user_file);
      return result;
      
    }
  }
  fclose(user_file);
  return "";
}

int get_user_count() {

  FILE *user_file;
  user_file = fopen("users.inp", "r");

  int count = 0;
  char buffer[10];
  while(fscanf(user_file, "%s", buffer) != EOF) {
    ++count;
  }

  fclose(user_file);

  return count;
}

void delete_call(int id) {
  FILE *data_file;
  FILE *data_temp;
  struct call call;

  data_file = fopen("database.dat", "r+");
  data_temp = fopen("temp.dat", "w");
  
  while (fread(&call, sizeof(struct call), 1, data_file)) {
    if (id == call.id) {
      printf("Gespräch wurde gelöscht!\n");
    }
    else {
      fwrite(&call, sizeof(struct call), 1, data_temp);
    }
  }

  fclose(data_file);
  fclose(data_temp);

  remove("database.dat");
  rename("temp.dat", "database.dat");
}

void delete_month() {

  int month, year;

  while(1){
    make_line(30);
    printf("Welcher Monat soll gelöscht werden? (MM/YYYY)\n");
    scanf("%d/%d", &month, &year);
    clear_input();

    if(is_correct_date(1, month, year)) {
      break;
    } else {
      system("clear");
      printf("[FEHLER] Ungültige Eingabe!\n");
    }
  }

  FILE *data_file;
  FILE *data_temp;
  struct call call;

  data_file = fopen("database.dat", "r+");
  data_temp = fopen("temp.dat", "w");

  int counter = 0;
  
  while (fread(&call, sizeof(struct call), 1, data_file)) {
    if ((year == call.beginn_date.year) && (month == call.beginn_date.month)) {
      ++counter;
    }
    else {
      fwrite(&call, sizeof(struct call), 1, data_temp);
    }
  }

  fclose(data_file);
  fclose(data_temp);

  remove("database.dat");
  rename("temp.dat", "database.dat");

  printf("%d Eintrag/Einträge wurde/n gelöscht!\n", counter);
  make_line(30);
}

int get_last_id() {
  int i = 0;
  FILE *data_file;
  data_file = fopen("database.dat", "r");

  struct call calls;
  while(fread(&calls, sizeof(struct call), 1, data_file)) {
    i = calls.id;
  }


  fclose(data_file);
  return i;
}

int is_correct_date(int day, int month, int year) {
  int month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  int success = 0;

  //Schaltjahr erkennen und Februar anpassen
  if(year % 400 == 0 || (year % 100 != 0 && year % 4 == 0)) {
   month_days[1] = 29;
  }

  if ((month<13) && (day <= month_days[month - 1])) {
    success = 1;
  }
  if ((day < 1) || (month < 1)) {
    success = 0;
  }

  return success;
}

int is_correct_time(int hour, int minute, int second) {
  int success = 0;

  if ((minute < 60) && (second < 60)) {
    success = 1;
  }

  if ((hour < 0) || (minute < 0) || (second < 0)) {
    success = 0;
  }

  return success;
}

float get_cost(char type[50], int duration_hour, int duration_minute, int duration_second, int beginn_hour, float costs[24][3]) {
  float call_costs;
  int cost_type;
  if (!strcmp(type, "Ort")) {
    cost_type = 0;
  } else if (!strcmp(type, "Fern")) {
    cost_type = 1;
  } else {
    cost_type = 2;
  }

  float duration = duration_hour*60*60+duration_minute*60+duration_second;

  call_costs = (duration/costs[beginn_hour][cost_type]) * 0.06;

  return call_costs;

}

void new_call() {
  struct call new_call;
  FILE *database;
  int user_id;

  new_call.id = get_last_id() + 1;

  database = fopen("database.dat", "a");
  while(1) {
    display_users();
    make_line(30);
    printf("Welcher Benutzer?\n");
    scanf("%i", &user_id);
    clear_input();

    if (!strcmp(get_user(user_id), "")) {
      system("clear");
      printf("[FEHLER] Unbekannte benutzer ID!\n");
      make_line(30);
    } else {
      new_call.caller_id = user_id;
      break;
    }
  }

  system("clear");
  printf("Benutzer: %s\n", get_user(user_id));

  while (1) {
    char type[50];

    make_line(30);
    printf("Art des Gesprächs? (Ort/Fern/Ausland)\n");
    scanf("%s", type);
    clear_input();

    if((!strcmp(type, "Ort")) || (!strcmp(type, "Fern")) || (!strcmp(type, "Ausland"))) {
      strcpy(new_call.destination, type);
      break;
    } else {
      system("clear");
      printf("[FEHLER] Unbekannte Art!\n");
    }
  }

  while (1) {

    int day,month,year;

    make_line(30);
    printf("Datum des Gesprächs? (TT/MM/YYYY)\n");
    scanf("%d/%d/%d", &day,&month,&year);
    clear_input();

    if(is_correct_date(day, month, year)) {
      new_call.beginn_date.year = year;
      new_call.beginn_date.month = month;
      new_call.beginn_date.day = day;
      break;
    } else {
      system("clear");
      printf("[FEHLER] Ungültiges Datum!\n");
    }
  }

  system("clear");
  printf("Datum Gespräch: %d/%d/%d\n", new_call.beginn_date.day, new_call.beginn_date.month, new_call.beginn_date.year);

  while (1) {

    int hour, minute;

    make_line(30);
    printf("Beginn des Gesprächs? (HH:MM)\n");
    scanf("%d:%d", &hour, &minute); //TODO
    clear_input();

    if(is_correct_time(hour, minute, 0)) {
      new_call.beginn_time.hour = hour;
      new_call.beginn_time.minute = minute;
      break;
    } else {
      system("clear");
      printf("[FEHLER] Ungültige Dauer!\n");
    }
  }

  system("clear");
  printf("Beginn Gespräch: %d:%d\n", new_call.beginn_time.hour, new_call.beginn_time.minute);

  while (1) {

    int hour, minute, second;

    make_line(30);
    printf("Dauer des Gesprächs? (HH:MM:SS)\n");
    scanf("%d:%d:%d", &hour, &minute, &second); //TODO
    clear_input();

    if(is_correct_time(hour, minute, second)) {
      new_call.duration.hour = hour;
      new_call.duration.minute = minute;
      new_call.duration.second = second;
      break;
    } else {
      system("clear");
      printf("[FEHLER] Ungültige Dauer!\n");
    }
  }

  system("clear");
  printf("Dauer Gespräch: %d:%d:%d\n", new_call.duration.hour, new_call.duration.minute, new_call.duration.second);
  
  fwrite(&new_call, sizeof(struct call),1, database);
  fclose(database);
}
