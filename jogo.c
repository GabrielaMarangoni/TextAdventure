#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// CONTROLES GERAIS
/////////////////////////////////////////////////////////////////////////
//#define DEBUG

#define TEXT_SHORT 50
#define TEXT_LONG  1000

#define MAX_COMMANDS 10
#define MAX_SCENES   20
#define MAX_ITEMS    50

// ESTRUTURAS NECESSARIAS
/////////////////////////////////////////////////////////////////////////

typedef struct command {
    char id[TEXT_SHORT];
    char type[TEXT_SHORT];
    char name[TEXT_SHORT];
    char result[TEXT_LONG];
    char outcome[TEXT_SHORT];
    char requirements[TEXT_LONG];
    char missingItems[TEXT_LONG];
} Command;

typedef struct scene {
    char id[TEXT_SHORT];
    char title[TEXT_SHORT];
    char description[TEXT_LONG];
    int commandsSize;
    Command commands[MAX_COMMANDS];
} Scene;

typedef struct item {
    char id[TEXT_SHORT];
    char name[TEXT_SHORT];
    char used[TEXT_SHORT];
} Item;

typedef struct game {
    int currentScene;
    int scenesSize;
    Scene scenes[MAX_SCENES];
    int itemsSize;
    Item items[MAX_ITEMS];
} Game;

typedef struct reliquia{
    char id[TEXT_SHORT];
    char name[TEXT_SHORT];
    char description[TEXT_LONG];
}Reliquia;
// OPERACOES
/////////////////////////////////////////////////////////////////////////

// global
Game game;
char feedbackitem[200]; //Guarda se o item está ou não requerido

void loadField(FILE* file, char field[], int n){
    char line[1000];
    //lê 499 caracteres e colocar NULL ('\0') no final
    //ou lê até encontrar \n (inclui o \n)
    fgets(line, 1000, file);

    int maxRead = strlen(line)-1; //-1 por conta do \n
    if(maxRead > n-1){
        printf("AVISO: Linha excedeu o máximo de caracteres: %s\n", line );
        maxRead = n-1;
    }
    strncpy(field, line, maxRead);
    field[maxRead] = '\0'; // termina a string, por garantia

    #ifdef DEBUG
    printf("line:  %s", line);
    printf("field: %s\n\n", field);
    #endif // DEBUG
  game.itemsSize = 0;
}

Command loadCommand(FILE* file){
    Command cmd;
    loadField(file, cmd.id, TEXT_SHORT);
    loadField(file, cmd.type, TEXT_SHORT);
    loadField(file, cmd.name, TEXT_SHORT);
    loadField(file, cmd.result, TEXT_LONG);
    loadField(file, cmd.outcome, TEXT_SHORT);
    loadField(file, cmd.requirements, TEXT_LONG);
    loadField(file, cmd.missingItems, TEXT_LONG);
    return cmd;
}

Scene loadScene(FILE* file){
    Scene s;
    loadField(file, s.id, TEXT_SHORT);
    loadField(file, s.title, TEXT_SHORT);
    loadField(file, s.description, TEXT_LONG);
    s.commandsSize =  0;
    return s;
    
}

void loadGameData(char fileName[]) {
    FILE* file;
    file = fopen(fileName,"r");

    char line[50];


    char str[] ="Loading game... Please wait"; //utilizar função sleep
        printf("\n\t\t\t");
        for (int i = 0; i < 28; i++){
            printf("%c",str[i]);
            Sleep(100);
            fflush(stdout);
        }printf("\n\t\t\t");
        for(int i=1; i < 28 ; i++){
           printf("/");
           Sleep(100);
           fflush(stdout); //atualiza a tela
        }
   printf("\n\n");

    if(file == NULL){
        printf("ERRO: arquivo não encontrado\n");
        return;
    }
    game.scenesSize = 0;
    Scene scn;

    while(fgets(line, sizeof(line), file)){
    //   if(strlen(line) == 0)
    //     continue;

        if(strstr(line, "CENA") != NULL){
            //ler uma cena
            scn = loadScene(file);

        }else if(strstr(line, "COMANDO") != NULL){
            //ler comando e adicionar a cena
            Command comm = loadCommand(file);
            scn.commands[scn.commandsSize] = comm;
            scn.commandsSize++;

        }else if(strstr(line, "END") != NULL){
            //terminou a cena -> adicionar a lista
            //cenas no game (game.scenes[])
            game.scenes[game.scenesSize] = scn;
            game.scenesSize++;
        }
    }

        fclose(file);
}

void printGameData(){
    for (int i = 0; i < game.scenesSize; i++){
        Scene s = game.scenes[i];
        printf("[SCENE]\n");
        printf(" %s\n %s\n %s\n\n", s.id, s.title, s.description);

        for (int j = 0; j < s.commandsSize; j++){
            Command c = s.commands[j];
            printf("   [COMMAND]\n");
            printf("    %s\n    %s\n    %s\n    %s\n    %s\n    %s\n    %s\n\n",
                c.id, c.type, c.name, c.result, c.outcome, c.requirements, c.missingItems);
        }
    }
}

void run(){ //rodar JOGO
    int i = 0, run = 1;
    int changeScene = 0;
    game.currentScene = 0;

            //se.id ....
    while(run){
      changeScene = 0;
        Scene se =  game.scenes[game.currentScene];

        printf("CENA[%s]:%s\n %s\n",se.id, se.title, se.description);

        char nop[200] = "NO";
        printf("\nInventário: [");
        for(int klb = 0; klb <= 10; klb++){
          char itenzinho[200];
          snprintf(itenzinho, 200, "%s", game.items[klb].used);
          if(strcmp(itenzinho, nop)== 0){
            printf("%s,", game.items[klb].name);
          }
        }
        printf("]\n\n");

        for(i = 0; i < se.commandsSize; i++){
            printf("[%d] - %s\n",i+1, se.commands[i].name);

        }
        printf("\n");
        printf("[10] - Salvar  /  [11] - Sair  /  [12] - Continuar com meus dados");
        printf("\n");
        while(!changeScene){
            int opcao = 0;

            printf("Comando: ");
            scanf(" %d", &opcao);
            if(opcao == 11){
              printf("\nAté a próxima!\n");
              exit(0);
            }else if(opcao == 10){
              FILE* filedata;
              char idCena[50];
              snprintf(idCena, 50, game.scenes[game.currentScene].id);
              filedata = fopen("data.txt", "w");
              fprintf(filedata, "CENA_PARADA\n%s\n\n", idCena);
              fprintf(filedata, "ITEM\n\n");
              for(int dataSave = 0; dataSave <= 10; dataSave++){
                char idItem[50];
                char NaItem[50];
                snprintf(idItem, 50, game.items[dataSave].id);
                snprintf(NaItem, 50, game.items[dataSave].name);
                fprintf(filedata, "%s\n", idItem);
                fprintf(filedata, "%s\n", NaItem);
              }
              fclose(filedata);
              printf("\nDados Salvos...\n");
            }

            if(opcao >= 1 && opcao <= se.commandsSize){

              //verifica se comands = COMANDO
              if(strlen(se.commands[opcao-1].type) == 7){
                printf("\n%s\n\n", se.commands[opcao-1].result);
                game.currentScene++;
                break;
              }

              //verifica se comands = ACTION
              else if(strlen(se.commands[opcao-1].type) == 6){
                if(strlen(se.commands[opcao-1].requirements) == 4){
                  printf("%s\n", se.commands[opcao-1].result);
                  if(strlen(se.commands[opcao-1].outcome) != 4){
                    char cena[200];
                    char cena0[200] = "titulo";
                    char cena1[200] = "inicio";
                    char cena2[200] = "sala";
                    char cena3[200] = "corredor";
                    char cena12[200] = "final2";
                    char cena10[200] = "destino";

                    snprintf(cena, 200, "%s", game.scenes[game.currentScene].id);
                    if(strcmp(cena, cena1) == 0){
                      if(opcao == 5){
                        //printf("\nIndo para nova cena\n");
                        printf("\n\n");
                        game.currentScene = 2;
                      }
                    }else if(strcmp(cena, cena3) == 0){
                      if(opcao == 1 || opcao ==3){
                        printf(" ____   ____   __  __  ____  ____ __  __ ____ _____\n");
                        printf("/ (_,` / ()   |   v  || ===|/ ()     v /| ===|| () )\n");
                        printf(" ____ __/  __ |_|  |_||____| ____/  __/ |____||_|L\n");
                        printf("[11] - sair   /     [12] - Voltar\n");
                        int opcao2 = 0;
                        printf("Comando: ");
                        scanf("%d", &opcao2);
                        if(opcao2 == 11){
                          exit(0);
                        }else if(opcao2 == 12){
                          game.currentScene = 3;
                          break;
                        }
                      }
                    }else if(strcmp(cena, cena10) == 0){
                      if(opcao == 1){
                        printf(" ____   ____   __  __  ____  ____ __  __ ____ _____\n");
                        printf("/ (_,` / ()   |   v  || ===|/ ()     v /| ===|| () )\n");
                        printf(" ____ __/  __ |_|  |_||____| ____/  __/ |____||_|L\n");
                        printf("[11] - sair   /     [12] - Voltar\n");
                        int opcao2;
                        printf("Comando: ");
                        scanf("%d", &opcao2);
                        if(opcao2 == 11){
                          exit(0);
                        }else if(opcao2 == 12){
                          game.currentScene = 10;

                          break;
                        }
                      }
                    }
                    if(game.currentScene == 10){
                      //printf("\nIndo para uma nova cena\n");
                      printf("\n\n");
                      game.currentScene = 11;
                    }
                    if(strcmp(cena, cena12) == 0){
                      exit(0);
                    }
                    break;
                  }else{
                    char cena[200];
                    char cena0[200] = "titulo";
                    snprintf(cena, 200, "%s", game.scenes[game.currentScene].id);
                    if(strcmp(cena, cena0) == 0){
                        if(opcao == 2)
                           exit(0);
                                                   }
                  }
                }else{
                  char feedbackitemOk[200] = "Ok";
                  char final[200];
                  char final2[200];
                  char final3[200];
                  char finalF[200];
                  char finalReq[200];
                  char usedY[200] = "Yes";

                  for (int it = 0; it < 50; it++) {
                    snprintf(finalReq, 200, "%s", se.commands[opcao-1].requirements);
                    for (int it2 = 0; it2 < 50; it2++) {
                      snprintf(final2, 200, "%s", game.items[it].id);
                      snprintf(final, 200, "%s", game.items[it2].id);
                      snprintf(finalF, 200, "%s, %s", game.items[it].id,game.items[it2].id);
                      if(strcmp(final, finalReq)==0){
                        snprintf(feedbackitem, 200, "%s", feedbackitemOk);
                        snprintf(game.items[it2].used, 200, "%s", usedY);
                      }
                      if(strcmp(finalF, finalReq)==0){
                        snprintf(feedbackitem, 200, "%s", feedbackitemOk);
                        snprintf(game.items[it2].used, 200, "%s", usedY);
                        snprintf(game.items[it].used, 200, "%s", usedY);
                      }
                    }
                  }
                  int p=0;
                    //printf("%s\n", feedbackitem);
                    //printf("%s\n", feedbackitemOk);
                    if(strcmp(feedbackitem, feedbackitemOk)!=0){
                      printf("%s\n", se.commands[opcao-1].missingItems);
                    }else if(strcmp(feedbackitem, feedbackitemOk)==0){
                      printf("%s\n", se.commands[opcao-1].result);
                      if(strlen(se.commands[opcao-1].outcome) != 4){
                        //printf("\nIndo para uma %s\n\n", se.commands[opcao-1].outcome);
                        char cena[200];
                        char cena0[200] = "titulo";
                        char cena1[200] = "inicio";
                        char cena2[200] = "sala";
                        char cena3[200] = "corredor";
                        char cena4[200] = "alucinacao";
                        char cena5[200] = "passado";
                        char cena6[200] = "cozinha";
                        char cena7[200] = "terror";
                        char cena8[200] = "torre";
                        char cena9[200] = "final1";
                        char cena10[200] = "destino";
                        char cena11[200] = "jantar";

                        snprintf(cena, 200, "%s", game.scenes[game.currentScene].id);
                        if(strcmp(cena, cena1) == 0){
                          if(opcao == 6){
                            game.currentScene = 3;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena1) == 0){
                          if(opcao == 5){
                            game.currentScene = 3;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena2) == 0){
                          if(opcao == 1){
                            game.currentScene = 10;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena3) == 0){
                          if(opcao == 2){
                            game.currentScene = 4;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena11) == 0){
                          if(opcao == 3){
                            game.currentScene = 9;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                            exit(0);
                          }
                        }else if(strcmp(cena, cena4) == 0){
                          if(opcao == 3){
                            game.currentScene = 5;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena5) == 0){
                          if(opcao == 2){
                            game.currentScene = 6;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena6) == 0){
                          if(opcao == 4){
                            game.currentScene = 7;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena7) == 0){
                          if(opcao == 3){
                            game.currentScene = 8;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");
                          }
                        }else if(strcmp(cena, cena8) == 0){
                          if(opcao == 5){
                            game.currentScene = 9;
                            char feedbackitemNo[200] = "O";
                            snprintf(feedbackitem, 200, "%s", feedbackitemNo);
                            printf("\n");

                          }
                        }else if(strcmp(cena, cena9) == 0){
                            exit(0);

                        }
                        break;
                    }
              }
            }

              //verifica se comands = ITEM
            }else if(strlen(se.commands[opcao-1].type) == 4){
                for (int w = game.itemsSize; w < 50; w++) {
                  if(w == game.itemsSize){
                    char used[3]= "NO";
                    sprintf(game.items[w].id, "%s", se.commands[opcao-1].id);
                    sprintf(game.items[w].name, "%s", se.commands[opcao-1].outcome);
                    sprintf(game.items[w].used, "%s", used);
                    game.itemsSize++;
                  }
                  break;
                }
              printf("%s\n",se.commands[opcao-1].result);
              }
        }else if(opcao != 10 && opcao != 11 && opcao != 12)
          printf("Comando inválido! Tente novamente!\n");
    }
  }
}

int main() {
    char nome[40];
    loadGameData("text_fiction.txt");
    //printGameData(); //testes
    run();
    char a;
    printf("\t\tReinicar?\n");
    printf("\t(s)SIM\t(n)Nao\n");


    while(a != 'N' && a != 'n' && a != 'S' && a != 's'){
      printf("Comando errado, tente novamente!\n" );
      scanf("Comando: %c\n", &a);
    }
    if(a == 'S' && a == 's') run();
    if(a ==  'N' && a == 'n') printf("Que pena...Ate logo...\n");

    return 0;
}
