#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fields.h"
#include "dllist.h"
#include "jval.h"

void listedenOutputDosyasina(Dllist ilkDugum, const char* dosyaAdi) {
  FILE *f = fopen(dosyaAdi, "w"); 
  
  if(f == NULL) {
    perror("Dosya Acma Hatasi!");
    return;
  }
  
  while(ilkDugum != NULL && ilkDugum->val.s != NULL) {
    if(strcmp(ilkDugum->val.s, "\\n") == 0) {
      fprintf(f, "%s", "\n"); 
    }
    else if(strcmp(ilkDugum->val.s, "\\b") == 0) {
      fprintf(f, "%s", "\b"); 
    }
    else {
      fprintf(f, "%s", ilkDugum->val.s); 
    }
    ilkDugum = ilkDugum->flink;
  }
  
  fclose(f);
}

int main(int argc, char **argv)
{
  IS is;
  Dllist dl;
  dl = new_dllist();
  dl->val.s = NULL;
  Dllist imlec = dll_first(dl); 

  if (argc != 3) { fprintf(stderr, "usage: input_file_name output_file_name\n");  exit(1); }
 
  is = new_inputstruct(argv[1]);
  
  
  if (is == NULL) {
    perror(argv[1]);
    exit(1);
  }

  // Satir bazli okuma
  while(get_line(is) >= 0) {
    Jval jv, str1, str2;
    jv.s = " "; 
    str1.s = "\\n";
    str2.s = "\\b";
  
    // Yazma komutu geldi
    if(strcmp(is->fields[0], "yaz:") == 0) {
      for(int i = 1; i < is->NF; i++) {
        int basilacakAdet = atoi(is->fields[i]); 
        // fields[i] integer deger ise 
        if(basilacakAdet != 0 || strcmp(is->fields[i], "0") == 0) {
        
          // Basilacak karakter \n ise
          if(strcmp(is->fields[i + 1], str1.s) == 0) {
            for(int i = 0; i < basilacakAdet; i++) {
              dll_insert_a(imlec, new_jval_s(strdup(str1.s))); 
              imlec = imlec->flink;
            }
          }
          
          // Basilacak karakter \b ise
          else if(strcmp(is->fields[i + 1], str2.s) == 0) {
            for(int i = 0; i < basilacakAdet; i++) {
              dll_insert_a(imlec, new_jval_s(strdup(str2.s))); 
              imlec = imlec->flink;
            }
          }
          
          // Basilacak karakter harfse
          else {
            jv.s = is->fields[i + 1];
            for(int i = 0; i < basilacakAdet; i++) {
              dll_insert_a(imlec, new_jval_s(strdup(jv.s))); 
              imlec = imlec->flink;
            }
          }
        }
      }
    }
    
    // Silme komutu geldi
    else if(strcmp(is->fields[0], "sil:") == 0) {
      Dllist temp;
      for(int i = 1; i < is->NF; i++) { 
        int silinecekAdet = atoi(is->fields[i]); 
          // fields[i] integer deger ise 
        if(silinecekAdet != 0 || strcmp(is->fields[i], "0") == 0) {
          imlec = dll_last(dl);
          while(strcmp(imlec->val.s, is->fields[i+1]) != 0 && imlec != dll_first(dl)) { 
            imlec = imlec->blink;
          }
          for(int i = 0; i < silinecekAdet; i++) {
            if(imlec->val.s != NULL) {
              temp = imlec;
              imlec = imlec->blink;
              free(temp->val.s);
              dll_delete_node(temp); 
            }
            else {
              break;
            }
          }
        } 
      } 
    }	
    
    // Sonagit komutu geldi
    else if(strcmp(is->fields[0], "sonagit:") == 0) {
      imlec = dll_last(dl);
    }
    
    // dur komutu geldi
    else if(strcmp(is->fields[0], "dur:") == 0) {
      Dllist ilkDugum = dll_first(dl);
      listedenOutputDosyasina(ilkDugum, argv[2]);
      
      Dllist sil = dll_first(dl);
      while(sil->flink!= dl) {
        free(sil->val.s);
        sil = sil->flink;
      }
      free(sil->val.s);
      free_dllist(dl);
      jettison_inputstruct(is);
      return 0; 
    }
    
    // Hatalı dosya icerigi
    else {
      printf("UYARI! GIRIS DOSYASI UYGUN FORMATTA DEGIL!\n");
      exit(1);
    }
  }
}

