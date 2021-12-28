
#include "mbed.h"
#include "stdio.h"
#include "SPI_TFT_ILI9341.h"
#include "stdlib.h"
#include "time.h"

#include "string"

//#include "Arial12x12.h"
#include "Arial24x23.h"
//#include "Arial28x28.h"
//#include "Neu42x35.h"
//#include "font_big.h"
//#include "SDFileSystem.h"
#define dp23 P0_0


//SPI_TFT_ILI9341 TFT(PTD2,PTD3,PTD1,PTD0,PTA20,PTA13,"TFT"); // mosi, miso, sclk, cs, reset, dc
SPI_TFT_ILI9341 TFT(dp2,dp1,dp6,dp24,dp23,dp25,"TFT");


#define xKraj 319
#define yKraj 239
#define xPtica 100
#define vrhZelenog 195
#define dnoZelenog 206
#define duzinaZelenog 80
#define pomakPticeY 1

#define ZelenaNaslov 47011
#define TamnoZelena 28133
#define SvijetloZelena 38698
#define NeboPlava 19993
#define Zemlja 54961
#define Zuto 63364


//InterruptIn t(A2);
InterruptIn t(dp9);


int pticaY, prosliPticaY;
float brzinaPtice(0), grav(1);
float frame(0.033333);
int stuboviXevi[3]; // = {320, 320, 320};
int stuboviY[3];
int prviZelPocetak, drugiZelPocetak;
int dijelic = 3; // za koliko se pomjere stubovi i zeleni dijelovi u jednom frameu
int maxBrzina (20), skokBrzina(-7);
int stanje(0);
int skor, bestSkor;
bool kolizija(false);
bool pocetak(true); // true do prvog pojavljivanja treceg stuba

void ns();


//++++++++++++++ Ptica


void crtajPticu() { // crtaj pticu
     
     // brisanje
     if (prosliPticaY == pticaY) // Nema potrebe za brisanjem
        return;
     if (prosliPticaY < pticaY) { // pomjerila se dolje // generalno: da li je brze ili sporije if (){return;} else()...
        TFT.fillrect(xPtica - 9, prosliPticaY - 6, xPtica + 7, pticaY - 7, NeboPlava); // od proslog pocetka do trenutnog
    } else { // pomjerila se gore
        TFT.fillrect(xPtica - 9, pticaY + 6, xPtica + 7, prosliPticaY + 5, NeboPlava); // od trenutnog kraja do proslog
    }
     
     // crtanje
     // za crtanje ptice promjenljiva je pticaY
     TFT.fillrect(xPtica-6,pticaY-5,xPtica,pticaY+4,63364); // zuto
     TFT.pixel(xPtica-6,pticaY-5,19993);
     TFT.fillrect(xPtica,pticaY-5,xPtica+4,pticaY-1,White); // bijelo
     TFT.pixel(xPtica+4,pticaY-5,19993);
     TFT.fillrect(xPtica-8,pticaY-2,xPtica-4,pticaY+1,White); // bijelo
     // horizontalne linije
     TFT.line(xPtica+1,pticaY,xPtica+6,pticaY,18856);
     TFT.line(xPtica+1,pticaY+2,xPtica+6,pticaY+2,18856);
     TFT.line(xPtica+1,pticaY+4,xPtica+5,pticaY+4,18856);
     TFT.line(xPtica+1,pticaY+1,xPtica+6,pticaY+1,62216);
     TFT.line(xPtica+1,pticaY+3,xPtica+5,pticaY+3,62216);
     TFT.line(xPtica-3,pticaY-6,xPtica+2,pticaY-6,18856);
     TFT.line(xPtica-8,pticaY-3,xPtica-5,pticaY-3,18856);
     TFT.line(xPtica-6,pticaY+4,xPtica-5,pticaY+4,18856);
     TFT.line(xPtica-5,pticaY-5,xPtica-4,pticaY-5,18856);
     // vertikalne linje
     TFT.line(xPtica+5,pticaY-3,xPtica+5,pticaY-1,18856);
     TFT.line(xPtica+3,pticaY-3,xPtica+3,pticaY-2,18856);
     TFT.line(xPtica-1,pticaY-4,xPtica-1,pticaY-2,18856);
     TFT.line(xPtica-3,pticaY-1,xPtica-3,pticaY,18856);
     TFT.line(xPtica-9,pticaY-2,xPtica-9,pticaY,18856);
     // pixeli
     TFT.pixel(xPtica,pticaY-1,18856);
     TFT.pixel(xPtica,pticaY+1,18856);
     TFT.pixel(xPtica,pticaY+3,18856);
     TFT.pixel(xPtica,pticaY-5,18856);
     TFT.pixel(xPtica-4,pticaY+1,18856);
     TFT.pixel(xPtica+7,pticaY+1,18856);
     TFT.pixel(xPtica-7,pticaY+3,18856);
     TFT.pixel(xPtica+6,pticaY+3,18856);
     TFT.pixel(xPtica-1,pticaY+2,18856);
     TFT.pixel(xPtica-4,pticaY-2,18856);
     TFT.pixel(xPtica-6,pticaY-4,18856);
     TFT.pixel(xPtica+3,pticaY-5,18856);
     TFT.pixel(xPtica+4,pticaY-4,18856);
     
     TFT.pixel(xPtica,pticaY+2,62216);
}

/*
void crtajPticu() {
    if (prosliPticaY == pticaY) // Nema potrebe za brisanjem
        return;

    if (prosliPticaY < pticaY) { // pomjerila se dolje // generalno: da li je brze ili sporije if (){return;} else()...
        TFT.fillrect(xPtica - 9, prosliPticaY - 6, xPtica + 7, pticaY - 7, NeboPlava); // od proslog pocetka do trenutnog
    } else { // pomjerila se gore
        TFT.fillrect(xPtica - 9, pticaY + 6, xPtica + 7, prosliPticaY + 5, NeboPlava); // od trenutnog kraja do proslog
    }
    // crtanje
    TFT.fillrect(xPtica - 9, pticaY - 6, xPtica + 7, pticaY + 5, Zuto);
    TFT.fillrect(xPtica + 3, pticaY - 3, xPtica + 4, pticaY, 18856);

    // alt brisanje: if (prosli != trenutni) TFT.fillrect(xPtica - 9, prosliPticaY - 6, xPtica + 7, prosliPticaY + 5, NeboPlava);
}
*/
void skok() { // flap
    brzinaPtice = skokBrzina;
}

//++++++++++++ Ptica^^^, update je kod tla



//********* TLO

void crtajZeleniDjelic(int pocetak) {
    TFT.fillrect(pocetak, vrhZelenog, pocetak + dijelic - 1, dnoZelenog, SvijetloZelena);
}

void brisiZeleniDjelic(int izaKraja) {
    TFT.fillrect(izaKraja, vrhZelenog, izaKraja + dijelic + 1, dnoZelenog, TamnoZelena);
}

void crtajCijeliZeleni(int pocetak) {
    crtajZeleniDjelic(pocetak);
    brisiZeleniDjelic(pocetak + duzinaZelenog);
    // TFT.fillrect(pocetak, vrhZelenog, pocetak + duzinaZel , dnoZelene, SvijetloZelena);
}

void crtajZeleniDijelovi(int pocetak) {
    brisiZeleniDjelic(duzinaZelenog - xKraj + pocetak - 1); // 0 + duzZ - ((xKraj - poc) + 1)
    crtajZeleniDjelic(pocetak);
    // TFT.fillrect(0, vrhZelenog, duzinaZel - xKraj + pocetak , dnoZelenog, SvijetloZelena);
    // TFT.fillrect(pocetak , vrhZelenog, xKraj, dnoZelenog, SvijetloZelena);
}

void crtajZelene() { // (int prviZelPocetak, int drugiZelPocetak)
    if (prviZelPocetak > xKraj - duzinaZelenog + 1) { // prvi u dijelovima
        crtajZeleniDijelovi(prviZelPocetak);
        crtajCijeliZeleni(drugiZelPocetak);
    } else if (drugiZelPocetak > xKraj - duzinaZelenog + 1) { // drugi u dijelovima
        crtajCijeliZeleni(prviZelPocetak);
        crtajZeleniDijelovi(drugiZelPocetak);
    } else { // oba su cijeli
        crtajCijeliZeleni(prviZelPocetak);
        crtajCijeliZeleni(drugiZelPocetak);
    }
}

void updatePticaZel() {
    // update y koordinate ptice, i pratecih varijabli
    brzinaPtice += grav;
    if (brzinaPtice > maxBrzina)
        brzinaPtice = maxBrzina;
    prosliPticaY = pticaY;
    pticaY += brzinaPtice * pomakPticeY;
    if (pticaY + 3 >= vrhZelenog) {
        kolizija = true;
    } else if (pticaY < 5) { // pri vrhu ekrana
        pticaY = 6;
    }

    
    // Update x koordinata zelenih
    prviZelPocetak -= dijelic;
    drugiZelPocetak -= dijelic;
    if (prviZelPocetak < 0) {
        prviZelPocetak = xKraj + 1 - dijelic;
    } else if (drugiZelPocetak < 0) {
        drugiZelPocetak = xKraj + 1 - dijelic;
    }
    
}

//************** Tlo^^^



//---------------- STUBOVI

void crtajStubove() {// promjena
    for (int i = 0; i < 3; i++) {
        // brisanje
        if (stuboviXevi[i] + 36 + dijelic < 320) {
            TFT.fillrect(stuboviXevi[i] + 36, 0, stuboviXevi[i] + 37 + dijelic, stuboviY[i] - 85, NeboPlava);
            TFT.fillrect(stuboviXevi[i] + 36, stuboviY[i], stuboviXevi[i] + 37 + dijelic, 194, NeboPlava);
        }
        // crtanje
        if (stuboviXevi[i]>= 0) {
            TFT.fillrect(stuboviXevi[i], 0, stuboviXevi[i] +dijelic, stuboviY[i] - 85, 26571);
            TFT.fillrect(stuboviXevi[i], stuboviY[i], stuboviXevi[i] + dijelic, 194, 26571);
        }
    }
} 



void updateStubovi() {
    int n(0); // broj aktivnih stubova
    if (pocetak && stuboviXevi[0] > 200)
        n = 1;
    else if (pocetak && stuboviXevi[0] > 81)
        n = 2;
    else {
        n = 3;
        pocetak = false;
    }

    for(int i(0); i < n; i++) {
        if (stuboviXevi[i] - dijelic >= -36) // mozda povecati sa -36 na vise, ili samo vece bez jednako
        {
            stuboviXevi[i] -= dijelic; // prva greska: bez (-)
            if (stuboviXevi[i] <= 100 && stuboviXevi[i] + dijelic > 100) { // dodano manjeJEDNAKO
                skor++;
                ns();
            }
            if (stuboviXevi[i] < 107 && stuboviXevi[i] + 35 > 91 &&     // stuboviXevi[i] < 107 && stuboviXevi[i] > 56
              (pticaY + 3 >= stuboviY[i] || pticaY - 6 <= stuboviY[i] - 86))  {
                kolizija = true;
                return;
            }
        }
        else
        {
            stuboviXevi[i] = 320;
            stuboviY[i] = rand()%35 + 115;  // opseg mozemo prosiriti ako treba
        }
    }
}

//---------------- Stubovi^


void nultoStanje() { // stanje 0: intro ekran
    TFT.background(Black);     // set background to black
    TFT.foreground(ZelenaNaslov);     // set chars to light green
    TFT.cls();
    TFT.set_font((unsigned char*) Arial24x23);
    TFT.locate(75, 98);
    printf("Flappy bird");
    wait(2);
}

void prvoStanje() { // stanje 1, ptica se ne pomjera
    // pocetne vrijednosti
    pticaY = 120;
    prviZelPocetak = duzinaZelenog * 2;
    drugiZelPocetak = duzinaZelenog * 4;

    crtajPticu();
    while(stanje == 1) { // dok je stanje 1 (mijenja se samo klikom)
        crtajZelene();
        updatePticaZel(); // mijenja i pticu, ali se ne prikazuje
        wait(frame);
    } 
}

void drugoStanje() { // Stanje 2, let ptice bez stubova
    pticaY = 120;
    kolizija = false;
    brzinaPtice = skokBrzina; // samo promijenili brzinu da bude skok

    for (int i = 0; i < 60; i++) { // 60 = 2s/frame, ne mozemo poravnati sa stubovima?
        crtajPticu();
        crtajZelene();
        updatePticaZel();
        if (i < 30) {
            TFT.fillrect(70, dnoZelenog + 1, 120, dnoZelenog + 2, 21508);
            TFT.fillrect(70, dnoZelenog + 3, 120, dnoZelenog + 4, 52552);
            TFT.fillrect(70, dnoZelenog + 5, 120, yKraj, Zemlja);
        }
        if (kolizija)
            return;
        wait(frame);
    }
}

void cetvrtoStanje() { // meni // promjena
    //TFT.set_font((unsigned char*) Arial24x23);
    if(skor > bestSkor)
        bestSkor = skor;
    TFT.fillrect(55, 80, 264, 191, 54961);
    TFT.locate(65, 100);
    printf("GAME OVER");
    TFT.locate(75, 130);
    printf("Score: %d", skor);
    TFT.locate(75, 160);
    printf("Best:  %d", bestSkor);
}

void ns() { // napisi skor // promjena (nova funkcija)
    int pozx(120);
    if(skor < 10)
        pozx = 155;
    else if(skor < 100)
        pozx = 148;
    else if(skor < 1000)
        pozx = 140;
    //TFT.fillrect(140,215,180,230,18856); // mozda zatreba
    TFT.locate(pozx, 215);
    printf("%d", skor);
}

void klik() {
    if (stanje ==  2 || stanje == 3) { // ako je stanje 2 ili 3; stanje & 2
        skok();
    } else if (stanje == 1) { // ako je stanje 1; stanje & 1
        stanje++;
    } else if (stanje == 4) { // stanje je 4; stanje & 4
        stanje = 1;
    }
}

int main() {

    //Inicijalizacija;
    TFT.claim(stdout);
    TFT.set_orientation(1);
    // enable = 0 ??
    
    srand(time(NULL));
    skor = 0;
    bestSkor = 0;

    stanje = 0;
    nultoStanje(); // intro ekran

    t.mode(PullUp);
    t.rise(&klik);

    // Stanja 1 do 4
    while(1) {
        // Crtanje pozadine
        TFT.fillrect(0, 0, xKraj, vrhZelenog - 1, NeboPlava);
        TFT.fillrect(0, vrhZelenog + 3, xKraj, dnoZelenog, TamnoZelena);
        TFT.fillrect(0, dnoZelenog + 1, xKraj, dnoZelenog + 2, 21508);
        TFT.fillrect(0, dnoZelenog + 3, xKraj, dnoZelenog + 4, 52552);
        // TODO: dodati linije, to su one dolje, modifikovati
        TFT.fillrect(0, dnoZelenog + 5, xKraj, yKraj, Zemlja);

        TFT.background(54961);
        TFT.foreground(Black);
        stanje = 1;
        prvoStanje();

        // nakon klika
        stanje = 2;
        drugoStanje(); // traje 2s ili do sudara sa tlom

        
        stanje = 3; // igranje sa preprekama
        for (int i = 0; i < 3; i++) {
            stuboviXevi[i] = 320;
            stuboviY[i] = rand()%70 + 100;
        }
        frame = 0.015;

        while (!kolizija)
        {
            crtajPticu();
            crtajStubove();
            crtajZelene();
            updatePticaZel();
            updateStubovi();
            wait(0.025);
        }

        // doslo do kolizije
        wait(1);
        stanje = 4;
        cetvrtoStanje();
        wait(4);
        // dodane dvije linije
        pocetak = true;
        skor = 0;
    }
}