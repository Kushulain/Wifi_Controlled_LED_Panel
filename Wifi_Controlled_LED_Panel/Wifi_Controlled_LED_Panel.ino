//TODO :
//fixer le décalage dans l'affichage de l'image
//Fixer le crash qui survient quand plusieurs images sont chargées, surement un rapport avec le todo précédent
//niveau de gris + gamma, en utilisant BITS_PER_PIXEL (necessite de modifier le javascript et de revoir la taille du buffer dans rBase64.cpp qui devra = à (COL*LIN*ROW*BITS_PER_PIXEL)/8)
//Verifier que la classe cooldown marche bien (j'ai utilisé que des délais jusque là, j'ai pas eu le tps de tester ma classe)
//

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "CoolDown.h"
#include "rBase64.h"

#define DEBUG_MODE 1

//attention modifier dans rBase64.cpp aussi
#define COL 96
#define LIN 7
#define ROW 9

#define CLK     13  // Clock
#define DATA     4  // Data
#define STR      5  // Displays what's stored in buffer (HIGH) or freeze (LOW)

#define MIN(x,y) (x < y ? x : y)

//Nombre de d'images en mémoire
#define MAX_FRAMES_COUNT 1

//Nombre de bits par pixel (> 1 : niveau de gris)
#define BITS_PER_PIXEL 1
#define POW_2_BPP_MINUS_1 1 // doit être égale  ==  (2^BITS_PER_PIXEL)-1    pour avoir le mask correspondant


const char* ssid = "JA-RdC";
const char* password = "SuperLapin";
//const char* ssid = "Colle Hok x10";
//const char* password = "feuillantinestkt";

//code html qui sera envoyé au client
const char* html = "<!DOCTYPE html><meta charset=utf-8><title>Ecran LED</title><meta content='Manipule l&#39;écran à LED du Jardin d&#39;Alice à distance. Penses à faire une backup de ton texte sur ta machine. ;)'name=description><h1>Ecran LED</h1><table><tr><td><p><input value='Défilement :'onclick=ChangeMode() type=button><td><p id=textmodeui>Par Page</table><p>Délai de rafraichissement <input id=delay-input value=1.0 onkeyup=CorrectAndApplyDelay()><p><label>Entrer le texte à afficher sur l&#39;écran : <input id=text-input value='Entrez votre texte...'> <button onclick=GenerateFromText() type=button>Générer</button><p><label for=files>Sélectionner des fichiers :</label><input id=files multiple type=file><p><button onclick=RemoveCurrentFrame() type=button>Suppr. Img.</button> <button onclick=FirstFrame() type=button>Première</button> <button onclick=PreviousFrame() type=button>Précédente</button> <button onclick=NextFrame() type=button>Suivante</button> <button onclick=LastFrame() type=button>Dernière</button><label id=pageid>0/0</p><button onclick=PostDatas() type=button>Envoyer</button><p></p><script>function FirstFrame(){wanted_ImageID=0,UpdatePreview()}function PreviousFrame(){wanted_ImageID-=1,UpdatePreview()}function NextFrame(){wanted_ImageID+=1,UpdatePreview()}function LastFrame(){wanted_ImageID=all_canvas.length-1,UpdatePreview()}function RemoveCurrentFrame(){displayed_ImageID>-1&&(document.body.removeChild(all_canvas[displayed_ImageID]),all_canvas.splice(displayed_ImageID,1),displayed_ImageID=-1),UpdatePreview()}function UpdatePreview(){all_canvas.length>0?(wanted_ImageID=Math.min(wanted_ImageID,all_canvas.length-1),wanted_ImageID=Math.max(wanted_ImageID,0),displayed_ImageID!=-1&&document.body.removeChild(all_canvas[displayed_ImageID]),document.body.appendChild(all_canvas[wanted_ImageID]),displayed_ImageID=wanted_ImageID):(displayed_ImageID!=-1&&document.body.removeChild(all_canvas[displayed_ImageID]),displayed_ImageID=-1),document.getElementById('pageid').innerHTML=displayed_ImageID+1+'/'+all_canvas.length,console.log(document.getElementById('pageid').innerHTML)}function CorrectAndApplyDelay(){for(var e=document.getElementById('delay-input').value,a=e.length-1;a>=0;a--)'0'!=e[a]&&'1'!=e[a]&&'2'!=e[a]&&'3'!=e[a]&&'4'!=e[a]&&'5'!=e[a]&&'6'!=e[a]&&'7'!=e[a]&&'8'!=e[a]&&'9'!=e[a]&&'.'!=e[a]&&(e=e.substring(0,a)+e.substring(a+1));document.getElementById('delay-input').value=e,refreshDelay=parseFloat(e),isNaN(refreshDelay)&&(refreshDelay=1,''!=e.length&&(document.getElementById('delay-input').value='1.0'))}function ChangeMode(){modeID=(modeID+1)%text_mode.length,document.getElementById('textmodeui').innerHTML=text_mode[modeID]}function img_create(e,a){var t=document.createElement('img');return t.src='ImagePreview',t.title='ImagePreview',t.width=e,t.height=a,t}function ClearCurrentFrames(){wanted_ImageID=0,displayed_ImageID!=-1&&document.body.removeChild(all_canvas[displayed_ImageID]),displayed_ImageID=-1,all_canvas=new Array}function GenerateFromText(){var e=0;inputText=document.getElementById('text-input').value;for(var a=0;e<inputText.length;)e=FillWithText(e,a),a++;UpdatePreview()}function FillWithText(e,a){var t=e,n=document.createElement('canvas');all_canvas.splice(Math.max(0,displayed_ImageID+1+a),0,n),n.width=2*LED_line_width,n.height=(LED_line_height+LED_line_spacing)*LED_line_count*2;for(var i=n.getContext('2d'),l=i.createImageData(n.width,n.height),r=l.data,o=LED_line_height*LED_line_width,d=(LED_line_height+LED_line_spacing)*LED_line_width,_=0;_<r.length;_+=4)_/16%d<o?(r[_+0]=0,r[_+1]=20,r[_+2]=0,r[_+3]=255):(r[_+0]=0,r[_+1]=0,r[_+2]=0,r[_+3]=255);for(var g=0,c=0,_=e;_<inputText.length;_++){t++;var s=g,m=c*(LED_line_height+LED_line_spacing),h=5*(inputText.charCodeAt(_)-32);console.log(5*(inputText.charCodeAt(_)-32));var D=-1;switch(h){case 970:h=325,D=310;break;case 1010:h=345,D=310;break;case 1095:h=425,D=310;break;case 1030:h=365,D=310;break;case 1060:h=395,D=310;break;case 1715:h=445,D=310;break;case 1005:h=345,D=470;break;case 960:h=325,D=320;break;case 1e3:h=345,D=320;break;case 1085:h=425,D=320;break;case 980:h=325,D=475;break;case 1015:h=345,D=475;break;case 1100:h=425,D=475;break;case 1035:h=365,D=475;break;case 1070:h=395,D=475;break;case 1115:h=445,D=475}for(var I=0;I<5;I++){var p=ASCII_TO_2D[h+I],u=0;D!=-1&&(u=ASCII_TO_2D[D+I]);for(var v=0;v<8;v++)if((u|p)&1<<v){var w=(m+v)*n.width*4*2+4*(s+I)*2;r[w]=100,r[w+1]=255,r[w+2]=50,r[w+3]=255,r[w+4]=50,r[w+5]=255,r[w+6]=50,r[w+7]=255,r[w+4*n.width]=50,r[w+4*n.width+1]=255,r[w+4*n.width+2]=50,r[w+4*n.width+3]=255,r[w+4*n.width+4]=50,r[w+4*n.width+5]=255,r[w+4*n.width+6]=50,r[w+4*n.width+7]=255}}if(g+=6,g>LED_line_width-6&&(g=0,c+=1,c>=LED_line_count))return i.putImageData(l,0,0),t}return i.putImageData(l,0,0),t}function PostDatas(){var e={};e.modeID=modeID,e.refreshDelay=refreshDelay;for(var a=0;a<all_canvas.length;a++)e['bitmap'+a]=btoa(String.fromCharCode.apply(null,MakeBitmap(all_canvas[a]))),console.log(e['bitmap'+a]);Post('',e)}function Post(e,a,t){t=t||'post';var n=new FormData;for(var i in a)a.hasOwnProperty(i)&&n.append(i,a[i]);var l=new XMLHttpRequest;l.open(t,e),l.send(n)}function MakeBitmap(e){for(var a=e.getContext('2d'),t=1,n=new Uint8Array(LED_line_count*LED_line_width*LED_line_height*t/8),i=0,l=8/t,r=(Math.pow(2,t)-1)/255,o=0,d=0,_=LED_line_count*LED_line_width*LED_line_height,g=0,c=LED_line_count-1;c>=0;c--)for(var s=0;s<LED_line_width;s++)for(var m=LED_line_height-1;m>=0;m--){var h=s,D=c*(LED_line_height+LED_line_spacing)+m,I=a.getImageData(2*h,2*D,1,1).data,p=Math.min(Math.pow(2,t)-1,Math.floor(r*Math.max(I[0],I[1],I[2])));o|=p<<t*i,i++,i!=l&&d!=_-1||(n[g]=o,o=0,i=0,g++),d++}return console.log('bitmap size : '+n.length),console.log('bitmap datas : '+n),n}function MakeImagePreview(e){for(var a=e.getContext('2d'),t=0;t<2*LED_line_width;t+=2)for(var n=0;n<(LED_line_height+LED_line_spacing)*LED_line_count*2;n+=2)if(n/2%(LED_line_height+LED_line_spacing)<LED_line_height){var i=a.getImageData(t,n,1,1).data;a.fillStyle='rgb(0,'+Math.max(i[0],i[1],i[2])+',0)',a.fillRect(t,n,2,2)}else a.fillStyle='rgb(0,0,0)',a.fillRect(t,n,2,2)}var LED_line_count=9,LED_line_spacing=5,LED_line_width=96,LED_line_height=7,modeID=0,text_mode=['Page par page','Défilement Vertical','Décalage'],all_canvas=new Array,inputText,wanted_ImageID=-1,displayed_ImageID=-1,refreshDelay=1,ASCII_TO_2D=[0,0,0,0,0,0,0,95,0,0,0,7,0,7,0,20,127,20,127,20,36,42,127,42,18,35,19,8,100,98,54,73,85,34,80,0,5,3,0,0,0,28,34,65,0,0,65,34,28,0,8,42,28,42,8,8,8,62,8,8,0,80,48,0,0,8,8,8,8,8,0,96,96,0,0,32,16,8,4,2,62,81,73,69,62,0,66,127,64,0,66,97,81,73,70,33,65,69,75,49,24,20,18,127,16,39,69,69,69,57,60,74,73,73,48,1,113,9,5,3,54,73,73,73,54,6,73,73,41,30,0,54,54,0,0,0,86,54,0,0,0,8,20,34,65,20,20,20,20,20,65,34,20,8,0,2,1,81,9,6,50,73,121,65,62,126,17,17,17,126,127,73,73,73,54,62,65,65,65,34,127,65,65,34,28,127,73,73,73,65,127,9,9,1,1,62,65,65,81,50,127,8,8,8,127,0,65,127,65,0,32,64,65,63,1,127,8,20,34,65,127,64,64,64,64,127,2,4,2,127,127,4,8,16,127,62,65,65,65,62,127,9,9,9,6,62,65,81,33,94,127,9,25,41,70,70,73,73,73,49,1,1,127,1,1,63,64,64,64,63,31,32,64,32,31,127,32,24,32,127,99,20,8,20,99,3,4,120,4,3,97,81,73,69,67,0,0,127,65,65,2,4,8,16,32,65,65,127,0,0,4,2,1,2,4,64,64,64,64,64,0,1,2,4,0,32,84,84,84,120,127,72,68,68,56,56,68,68,68,32,56,68,68,72,127,56,84,84,84,24,8,126,9,1,2,8,20,84,84,60,127,8,4,4,120,0,68,125,64,0,32,64,68,61,0,0,127,16,40,68,0,65,127,64,0,124,4,24,4,120,124,8,4,4,120,56,68,68,68,56,124,20,20,20,8,8,20,20,24,124,124,8,4,4,8,72,84,84,84,32,4,63,68,64,32,60,64,64,32,124,28,32,64,32,28,60,64,48,64,60,68,40,16,40,68,12,80,80,80,60,68,100,84,76,68,0,8,54,65,0,0,0,127,0,0,0,65,54,8,0,0,4,2,1,0,0,1,0,1,0],imgs=new Array;window.onload=function(){if(window.File&&window.FileList&&window.FileReader){var e=document.getElementById('files');e.addEventListener('change',function(e){var a=e.target.files;console.log('heyy',a);for(var t=0;t<a.length;t++){var n=a[t];if(n.type.match('image')){var i=document.createElement('canvas'),l=all_canvas.push(i)-1;i.width=2*LED_line_width,i.height=(LED_line_height+LED_line_spacing)*LED_line_count*2;var r=(i.getContext('2d'),new Image),o=imgs.push(r)-1;r.onload=function(e,a){return function(){console.log('yièt',imgs[e]),all_canvas[a].getContext('2d').drawImage(imgs[e],0,0,2*LED_line_width,(LED_line_height+LED_line_spacing)*LED_line_count*2),console.log('loaded'),MakeImagePreview(all_canvas[a]),UpdatePreview()}}(o,l),console.log('lol'),imgs[o].src=URL.createObjectURL(n),console.log('img:',imgs[o])}}})}else console.log('Your browser does not support File API');console.log('Crap')}</script>";

ESP8266WebServer server(80);

//class stockant les données des images
class ImageLED
{
public:
  //Données de l'image
  unsigned char datas[(COL*LIN*ROW*BITS_PER_PIXEL)/8];

  ImageLED() {};
};

//Nombre d'images chargées
int imageCount = 0;

//Index de l'image actuellement affichée
int curr_Frame = 0;

//Images chargées
ImageLED images[MAX_FRAMES_COUNT];

//Delai Rafraichisement
CoolDown refreshCooldown = CoolDown(1000);

//Type de Scrolling : 0 : Page par page 1 : Défilement Vertical 2 : Décalage   //TODO non implémenté
byte scrollingType = 0;

//URI Incorrect
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(404, "text/plain", message);
}


//Executée à la reception d'un "POST", charge les données (délai,défilement,images)
void HandleMsg()
{


  if (server.method() == HTTP_GET)
  {
    server.send(200, "text/html", html);
  }
  else
  {
    String message = " ";
    int newImageCount = 0;

    Serial.print("Found POST with keys : ");
    for (uint8_t i=0; i<server.args(); i++)
    {

      Serial.print(server.argName(i) + ", ");
      if (server.argName(i).startsWith("bitmap", 0))
      {
        newImageCount++;
      }
    }
    Serial.println(" (total of " + String(newImageCount) + " images)");

    imageCount = MIN(MAX_FRAMES_COUNT,newImageCount);

    Serial.print("image Count : ");
    Serial.println(imageCount);

    int curLoadedFrame = 0;

    for (uint8_t i=0; i<server.args(); i++)
    {
      if (server.argName(i) == "modeID")
      {
        scrollingType = server.arg(i).toInt();
      }
      else if (server.argName(i) == "refreshDelay")
      {
        refreshCooldown.SetCooldown(server.arg(i).toFloat()*1000.0);
      }
      else if (server.argName(i).startsWith("bitmap", 0))
      {
        char* decoded = rbase64.decode_to_char(server.arg(i));
        size_t decoded_length = rbase64_dec_len((char *)(server.arg(i).c_str()), server.arg(i).length());

        #ifdef DEBUG_MODE
        Serial.println("Base64 length is : " + String(server.arg(i).length()) + " decoded length is : " + String(decoded_length));
        #endif
                
        if (decoded_length != (COL*LIN*ROW*BITS_PER_PIXEL)/8)
        {
          Serial.println("Erreur  : La taille de l'image reçue " + String(decoded_length) + " est diffèrente de celle attendue : " + String((COL*LIN*ROW*BITS_PER_PIXEL)/8));
        }
        int str_length = MIN(decoded_length,(COL*LIN*ROW*BITS_PER_PIXEL)/8);

        
        int testInt = 0;
        
        #ifdef DEBUG_MODE
        Serial.print("encoded image data : ");
        Serial.println((server.arg(i)));
        Serial.println(" ");
        
        Serial.print("decoded image data : ");
        Serial.println(decoded);
        Serial.println(" ");
        #endif

        for (int k=0; k<str_length; k++)
        {
          //copie le char* décodé vers le char* des images
          images[curLoadedFrame].datas[k] = decoded[k];

          // outputs binary datas :
          #ifdef DEBUG_MODE
          Serial.print(decoded[k],BIN);
          Serial.print(":");
          Serial.print((unsigned char)decoded[k]);
          Serial.print(" ");
          testInt++;
          if (testInt == 84)
          {
            Serial.println(" ");
            testInt = 0;
          }
          #endif
        }
        curLoadedFrame++;
      }
    }
    Serial.println( "POST Fin");
  }
}

//Format actuel des données des pixel envoyé depuis le js.
//Les valeures de pixels sont simplement intégré bit à bit à la suite dans une chaine de byte.
//En commençant par le bit de poids faible
//
//    avec 1 bit par pixel :
//     ID pixels                   7  6  5  4  3  2  1  0       15 14 13 12 11 10  9  8       23 22 21 20 19 18 17 16       ...
//     Données (octets) :       [  0  1  1  0  0  1  0  1  ]  [  0  1  1  0  0  1  0  1  ]  [  0  1  1  0  0  1  0  1  ]    ...
//
//    avec 2 bit par pixel :
//     ID pixel         [  3  ][  2 ][  1 ][  0 ]     [  7  ][  6 ][  5 ][  4 ]     [  11 ][  10][  9 ][  8 ]     ...
//     Données :       [  0  1  1  0  0  1  0  1  ]  [  0  1  1  0  0  1  0  1  ]  [  0  1  1  0  0  1  0  1  ]   ...

void render2panel(int frame)
{
  // Render content of framebuffer to screen
  int pixels_per_byte = 8/BITS_PER_PIXEL;
  int extraLED = 0;

  //Pour chaque pixel du panneau...
  for (int pixelID=0; pixelID<(ROW*COL*LIN); pixelID++)
  {
    //...on trouve d'abord le byte correspondant
    int byteId = pixelID/pixels_per_byte;
    //puis le 1er bit concernant le pixel dans le byte
    int spotInByte = BITS_PER_PIXEL*(pixelID%pixels_per_byte);
    //puis on extrait sa valeur grâce a un décalage et un masque
    bool pixelValue = (((images[frame].datas[byteId] >> spotInByte) & POW_2_BPP_MINUS_1) > 0 );

    digitalWrite(CLK, LOW);
    digitalWrite(DATA, pixelValue ? HIGH : LOW);
    digitalWrite(CLK, HIGH);

    //Code censé ajouter le pixel invisible tout les 7 pixels
    extraLED++;
    if (extraLED == 7)
    {
      digitalWrite(CLK, LOW); digitalWrite(CLK, HIGH);
      extraLED = 0;
    }
  }
  digitalWrite(STR, HIGH);   // Displays what's in registers
  delay(1);
  digitalWrite(STR, LOW);    // Freezes display

  Serial.println("D");
}

void setup(void){
  refreshCooldown = CoolDown(1000);

  Serial.begin(115200);
  Serial.println("letsGo");
  pinMode(CLK, OUTPUT);      // CP (Clock)
  pinMode(STR, OUTPUT);      // STR
  pinMode(DATA, OUTPUT);     // D (Data)


  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HandleMsg);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();

  delay(1000);

  Serial.println("loop");

  //Le TryGo() agit comme un délai sans freeze le programme
  if (refreshCooldown.TryGo() && imageCount > 0)
  {
    Serial.println("RenderPanel");
    render2panel(curr_Frame);
    curr_Frame = (curr_Frame+1)%imageCount;
  }

}

