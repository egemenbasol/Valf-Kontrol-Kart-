#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C ekran(0x27,16,2); //SDA A4 SCL A5 \\

boolean buton_durum= LOW, buton_eski_durum = LOW ,sistem_calis = false, dongu_bitti = true;
unsigned long yeni_zaman=0,eski_zaman=0;
unsigned long ekran_yeni_zaman= 0, ekran_eski_zaman = 0;
unsigned long buton_yeni_zaman = 0, buton_eski_zaman = 0;
int bekleme_suresi = 0, adim=-1;


//**************Değiştirilebilir kısım****************//
int buton1 = 10;
int buton2 = 12 ;
boolean seri_port = false, lcd_ekran = true, pull_down = true; // Seri portta bilgilerin yazılıp yazılmayacağı i2c ekrana yazılmayacağı ve buton direncinin pull down olup olmadığı
float buton_bekleme_suresi = 1000; //Buton basıldıktan sonra ne kadar süre butonu okumayacağını düzenliyor ms cinsinden
int ekran_bekleme_suresi = 500; //Ekranın ne kadar sürede yenileneceği ms cinsinden

#define led 13

//**************Değiştirilebilir kısım****************//
void setup() {
  pinMode(2,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  pinMode(buton1,INPUT) ;
  pinMode(buton2,INPUT) ;
  Serial.begin(9600);
  ekran.init();
  ekran.backlight();
}

void sistem_adimlar(){
  adim ++;
  switch(adim){
    default:
      adim = -1;
      bekleme_suresi = 0;
      return;
    case 0:
      digitalWrite(2,HIGH);
      bekleme_suresi = 500;
      break;
    case 1:
      digitalWrite(2,LOW);
      bekleme_suresi = 500;
      break;
    case 2:
      digitalWrite(5,HIGH);
      bekleme_suresi = 1000;
      break;
    case 3:
      digitalWrite(5,LOW);
      bekleme_suresi = 1000;
      break;
    case 4:
      digitalWrite(8,HIGH);
      bekleme_suresi = 2000;
      break;
    case 5:
      digitalWrite(8,LOW);
      bekleme_suresi = 2000;
      break;
  }
}

void buton_kontrol(){
  buton_durum = pull_down ? digitalRead(buton1): !digitalRead(buton1);
  buton_yeni_zaman = millis();
  if(buton_durum == HIGH and buton_eski_durum == LOW and buton_yeni_zaman - buton_eski_zaman >= buton_bekleme_suresi - 0.001){
    buton_eski_durum = HIGH;
    sistem_calis = !sistem_calis;
    digitalWrite(led,sistem_calis);
    buton_eski_zaman = buton_yeni_zaman;
  }
  else if(buton_durum == LOW and buton_eski_durum == HIGH and buton_yeni_zaman - buton_eski_zaman >= buton_bekleme_suresi - 0.001){
    buton_eski_durum = LOW;
    buton_eski_zaman = buton_yeni_zaman;
  }
}

void ekrana_goster(){
  ekran_yeni_zaman = millis();
  int saniye = ekran_yeni_zaman/1000.0;
  int dakika = saniye/60, saat = dakika/60;
  saniye %= 60; dakika %= 60; saat %= 24;
  if(ekran_yeni_zaman - ekran_eski_zaman > ekran_bekleme_suresi){
    if(seri_port){
      Serial.print("Sistem Durumu: "); Serial.println(sistem_calis ? "Aktif" : "Pasif");
      Serial.print("Suanda "); adim >=0 ? Serial.print(adim) : Serial.print(adim+1); Serial.println(". adimda");
      Serial.print("Calisma suresi: "); 
      Serial.print(saat); Serial.print(":"); Serial.print(dakika); Serial.print(":"); Serial.print(saniye); Serial.println("\tSaat/Dakika/Saniye");
      Serial.println("**********************************************");
    }
    if(lcd_ekran){
      ekran.clear(); ekran.print("Sistem: "); ekran.print(sistem_calis ? "Aktif " : "Pasif"); ekran.setCursor(0,1); ekran.print(saat); ekran.print(":");
      ekran.print(dakika); ekran.print(":");ekran.print(saniye); ekran.print(" Sa/D/Sn");
    }
    ekran_eski_zaman = ekran_yeni_zaman;
  }
  
}
void loop() 
{
  ekrana_goster();
  buton_kontrol();
  if(sistem_calis == true || dongu_bitti == false){
    dongu_bitti = false;
      buton_kontrol();
      yeni_zaman = millis();
      if(yeni_zaman - eski_zaman >= bekleme_suresi - 0.01){
        sistem_adimlar();
        eski_zaman = yeni_zaman;
        adim == 0 ? dongu_bitti = true: dongu_bitti = false;}
  }
  if(sistem_calis == false && dongu_bitti == true)
  {
    adim = -1, bekleme_suresi = 0;
     digitalWrite(2,LOW);
     digitalWrite(5,LOW);
     digitalWrite(8,LOW);
  }

}
