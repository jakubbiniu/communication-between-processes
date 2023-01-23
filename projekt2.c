#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys//stat.h>
#include<stdlib.h>
#include<errno.h>

char *first_word(char *str){
    int i;
    static char word[1024];
    memset(word,0,sizeof(word));
    for(i=0; str[i] != ' '; i++)
    {
        word[i] = str[i];
    }
    word[i] = '\0';
    return word;
}

char *last_word(char *str){
    int start,stop;
    static char word[1024];
    memset(word,0,sizeof(word));
    stop = strlen(str)-1;
    for(int i=stop;i>=0;i--){
        if((int)str[i]==32){
            start = i+1;
            break;
        }
    }
    int y=0;
    for(int x=start;x<=stop;x++){
        word[y]=str[x];
        y++;
    }
    word[y]='\0';
    return word;
}

char *middle_part(char *str){
    int start, stop;
    static char word[1024];
    memset(word,0,sizeof(word));
    for (int i=0;i<strlen(str);i++){
        if ((int)str[i]==32){
            start=i+1;
            break;
        }
    }
    for(int i=strlen(str)-1;i>=0;i--){
        if((int)str[i]==32){
            stop= i-1;
            break;
        }
    }
    int x=0;
    for (int i=start;i<=stop;i++){
        word[x]=str[i];
        x++;
    }
    return word;
}

char *first_part(char *str){
    int stop;
    static char word[1024];
    memset(word,0,sizeof(word));
    for(int i=strlen(str)-1;i>=0;i--){
        if((int)str[i]==32){
            stop= i-1;
            break;
        }
    }
    for (int x=0;x<=stop;x++){
        word[x]=str[x];
    }
    return word;
}

int main(int argc, char* argv[]){
    char process_name[1024];
    strcpy(process_name,argv[1]);


    // czytamy plik konfiguracyjny, żeby znaleźć nazwe kolejki fifo
    // Jesli znajdziemy nazwe to tworzymy kolejke, w przeciwnym razie wypisujemy że nie istnieje w pliku konfiguracyjnym
    int config = open("config.txt",O_RDONLY);
    char r, process[1024], fifo[1024], fifo_name[1024], result[1024];
    int i=0, found=0, read_fifo=0;
    while(read(config, &r, sizeof(r))>0){
        // printf("%c",r);
        if (r=='\n'){
            read_fifo=0;
            // printf("p: %s fi: %s\n", process, fifo);
            if (strcmp(process,process_name)==0){
                found=1;
                strcpy(fifo_name,fifo);
                break;
            }
            else{
                memset(process,0,sizeof(process));
                memset(fifo,0,sizeof(fifo));
                i=0;
            }
        }
        else if((int)r==32){
            lseek(config, 2, SEEK_CUR);
            read_fifo = 1;
            i=0;
        }
        else if(read_fifo==0){
            process[i]=r;
            i++;
        }
        else if(read_fifo==1){
            fifo[i]=r;
            i++;
        }
    }
    if (found==0){printf("nie znaleziono kolejki\n");}
    else{printf("nazwa kolejki to: %s\n", fifo_name);}
    close(config);


    // teraz tworze kolejke fifo o znalezionej nazwie
    mkfifo(fifo_name, 0666);
    int to_kill=0;

    // teraz w drugim procesie odczytuje polecenie
    int f = fork();
    if (f==0){
        // printf("potomny:%d\n",getpid());
        // sleep(2);
        char process2_name[1024], instruction[1024], fifo2_name[1024], line[4096];
        printf("wpisz polecenie do wykonania w formacie:proces komenda kolejka, delete-usuwam kolejke\n");
        while(1){
            memset(process2_name,0,sizeof(process2_name));
            memset(fifo2_name,0,sizeof(fifo2_name));
            memset(instruction,0,sizeof(instruction));
            i=0;
            fflush(stdin);
            memset(line,0,sizeof(line));
            scanf("%[^\n]%*c", line);
            // printf("1%s1\n", line);
            if (strcmp(line,"delete")==0){
                unlink(fifo_name);
                to_kill=1;
                kill(getpid(),SIGINT);
                return 0;
            }

            memset(process2_name,0,sizeof(process2_name));
            memset(instruction,0,sizeof(instruction));
            memset(fifo2_name,0,sizeof(fifo2_name));
            strcpy(process2_name,first_word(line));
            strcpy(instruction,middle_part(line));
            strcpy(fifo2_name,last_word(line));

            printf("proces do ktorego wysylam: %s, komenda: %s, nazwa nowej kolejki:%s\n", process2_name, instruction, fifo2_name);


            // teraz szukamy w pliku konfiguracyjnym nazwy kolejki procesu do ktorego wysylamy polecenie
            config = open("config.txt",O_RDONLY);
            char process3[1024], fifo3[1024], fifo3_name[1024];
            memset(fifo3,0,sizeof(fifo3));
            memset(process3,0,sizeof(process3));
            i=0, found=0, read_fifo=0;
            while(read(config, &r, sizeof(r))>0){
                // printf("%c",r);
                if (r=='\n'){
                    read_fifo=0;
                    // printf("p: %s fi: %s\n", process, fifo);
                    if (strcmp(process,process2_name)==0){
                        found=1;
                        strcpy(fifo3_name,fifo);
                        break;
                    }
                    else{
                        for (i=0;i<strlen(process);i++){process[i]='\0';}
                        for (i=0;i<strlen(fifo);i++){fifo[i]='\0';}
                        i=0;
                    }
                }
                else if(r==' '){
                    lseek(config, 2, SEEK_CUR);
                    read_fifo = 1;
                    i=0;
                }
                else if(read_fifo==0){
                    process[i]=r;
                    i++;
                }
                else if(read_fifo==1){
                    fifo[i]=r;
                    i++;
                }
            }
            if (found==0){
                printf("nie znaleziono kolejki na ktora mam wyslac polecenie\n");
            }
            else{
                printf("nazwa kolejki na ktora wysylam polecenie to: %s\n", fifo3_name);
            }
            close(config);


            // wysylam przez kolejke polecenie do danego procesu a takze tworze kolejke do otrzymania wyniku
            int k3 = open(fifo3_name,O_WRONLY);
            if (k3<0){printf("nie udalo sie otworzyc kolejki\n");}
            strcat(instruction," ");
            strcat(instruction, fifo2_name);
            mkfifo(fifo2_name, 0666);
            int w = write(k3, &instruction, sizeof(instruction));
            if(w<0){printf("nie zapisano do kolejki\n");}
            // printf("wyslano tyle bajtow:%d\n",w);
            close(k3);


            //odczytuje wykonane polecenie z kolejki o stworzonej nazwie
            int k2 = open(fifo2_name, O_RDONLY);
            if(k2<0){printf("nie otwarto kolejki do odczytu wyniku komendy\n");}
            i=0;
            // printf("zacyznam czytac\n");
            memset(result,0,sizeof(result));
            while(read(k2,&r,1)>0){
                result[i]=r;
                i++;
            }
            // printf("przeczytano\n");
            printf("Odczytany wynik: %s\n",result);
            memset(result,0,sizeof(result));
            close(k2);
            unlink(fifo2_name);
        }
    }


    // w tym procesie nasluchuje i czekam na komendy od innych, jesli dostane to czytam komende i otwieram kolejke
    else{
        // printf("macierzysty:%d\n",getpid());
        if (to_kill==1){
            kill(getpid(), SIGTERM);
            return 0;
        }
        // printf("jestem w else\n");
        int k1 = open(fifo_name, O_RDONLY);
        if (k1<0){printf("nie udalo sie otworzyc globalnej kolejki gdzie nasluchuje\n");}
        char buf, buf2[4096], fifo_to_send[4096], instruction_to_do[4096];
        int count=0,i=0;
        while(1){
            if (to_kill==1){
                break;
                kill(getpid(),SIGINT);
                return 0;
            }
            memset(fifo_to_send,0,sizeof(fifo_to_send));
            memset(instruction_to_do,0,sizeof(instruction_to_do));
            i=0;
            buf='\0';
            memset(buf2,0,sizeof(buf2));
            r = read(k1,&buf2,sizeof(buf2));
            if(buf2[0]!='\0'){
                // printf("odebralem:%s\n",buf2);
                strcpy(instruction_to_do,first_part(buf2));
                strcpy(fifo_to_send,last_word(buf2));
                memset(buf2,0,sizeof(buf2));
                if(fifo_to_send[0]!='\0'){
                    // printf("komenda:%s|fifo:%s!!!\n", instruction_to_do,fifo_to_send);
                    int k4 = open(fifo_to_send, O_WRONLY);
                    if (k4<0){
                        perror("nie udalo sie otworzyc kolejki gdzie mam wyslac");
                        printf("%d\n", errno);
                    }
                    else{
                        // printf("otwarto kolejke\n");
                        FILE *ls_cmd = popen(instruction_to_do, "r");
                        if (ls_cmd == NULL)
                            {
                            fprintf(stderr, "popen(3) error");
                            exit(EXIT_FAILURE);
                        }
                        static char result_buff[1024];
                        size_t n;
                        while ((n = fread(result_buff, 1, sizeof(result_buff) - 1, ls_cmd)) > 0) {   
                        result_buff[n] = '\0';
                        }
                        if (pclose(ls_cmd) < 0) perror("pclose(3) error");
                        for (i=0;i<strlen(fifo_to_send);i++){fifo_to_send[i]='\0';}
                        // printf("wysylam:%s\n",result_buff);
                        write(k4, &result_buff, sizeof(result_buff));
                        close(k4);
                        printf("zakonczono wysylanie\n");
                        // close(1);
                        // dup(f);
                        // execlp(instruction_to_do, instruction_to_do, NULL);
                    }
                }      
            }
        }
        close(k1); 
    }
    return 0;
}
