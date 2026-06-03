int btn=3,snd=2,bz=5,vib=4;
bool arm=0,alrm=0,lst=1;
void setup(){
  pinMode(btn,INPUT_PULLUP);
  pinMode(vib,INPUT_PULLUP);
  pinMode(snd,INPUT);
  pinMode(bz,OUTPUT);
}
void loop(){
  bool btnSt=digitalRead(btn);
  if(btnSt==LOW && lst==HIGH){
    delay(300);
    if(alrm||arm){arm=0;alrm=0;noTone(bz);}
    else{arm=1;for(int i=0;i<3;i++){tone(bz,2000,50);delay(100);}}
  }
  lst=btnSt;
if(arm && !alrm && (digitalRead(snd)==HIGH || digitalRead(vib)==HIGH)) alrm=1;
if(alrm){
  tone(bz,1000,200);delay(200);
  tone(bz,600,200);delay(200);}
}
