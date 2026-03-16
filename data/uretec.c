/* data/uretec.c - Opsiyonel veri uretici (+5 puan) */
#include <stdio.h>
#include <stdlib.h>

const char* SANATCILAR[] = {
    "Radiohead", "Daft Punk", "Portishead", "Massive Attack", "Boards of Canada",
    "The National", "Bon Iver", "Burial", "Four Tet", "Aphex Twin",
    "Bicep", "Floating Points", "Moderat", "Nils Frahm", "Bonobo",
    "Tycho", "Caribou", "Nicolas Jaar", "James Blake", "Kiasmos",
    "Jon Hopkins", "Max Cooper", "Rival Consoles", "Christian Loffler", "Benoit & Sergio",
    "Darkside", "Stimming", "David August", "Panda Bear", "Animal Collective",
    "Grizzly Bear", "Fleet Foxes", "Arcade Fire", "LCD Soundsystem", "The Strokes",
    "Arctic Monkeys", "Interpol", "Editors", "The Killers", "White Lies",
    "Tame Impala", "Mac DeMarco", "Unknown Mortal Orchestra", "MGMT", "Foster The People",
    "Empire of the Sun", "Phoenix", "Justice", "Kavinsky", "SebastiAn"
};

const char* ALBUMLER[] = {
    "OK Computer", "Discovery", "Dummy", "Mezzanine", "Geogaddi",
    "Alligator", "For Emma", "Untrue", "There Is Love", "Selected Ambient",
    "Bicep", "Crush", "Moderat II", "Spaces", "Black Sands",
    "Awake", "Our Love", "Space Is Only Noise", "Overgrown", "Blurred",
    "Immunity", "Human", "Howl", "A Forest", "Boy",
    "Psychic", "Alpe Lusia", "Times", "Person Pitch", "Merriweather",
    "Veckatimest", "Helplessness", "The Suburbs", "Sound of Silver", "Is This It",
    "AM", "Turn On The Bright Lights", "The Back Room", "Hot Fuss", "To Lose My Life",
    "Currents", "Salad Days", "Multi-Love", "Oracular", "Torches",
    "Walking On A Dream", "Wolfgang", "Cross", "OutRun", "Total",
    "Abbey Road", "The Dark Side", "Led Zeppelin IV", "Rumours", "Thriller",
    "Back in Black", "The Wall", "Sgt. Pepper", "Hotel California", "Nevermind",
    "Ten", "Metallica", "Appetite", "The Joshua Tree", "Born in the USA",
    "Brothers in Arms", "Synchronicity", "Purple Rain", "Like a Virgin", "True Blue",
    "Bad", "Dangerous", "Faith", "No Jacket", "Off the Wall",
    "Bridge", "Let It Be", "Sticky Fingers", "Exile", "London Calling",
    "The Clash", "Never Mind", "Ramones", "Unknown Pleasures", "Closer",
    "The Queen", "Meat Is Murder", "Disintegration", "Pornography", "The Head",
    "Violator", "Music for the Masses", "Black Celebration", "Some Great", "Speak & Spell",
    "A Broken Frame", "Construction Time", "Ultra", "Exciter", "Playing the Angel"
};

void veri_uret(const char* dosya, int n) {
    srand(42);  /* sabit seed -> her calistirmada ayni veri */
    FILE* f = fopen(dosya, "w");
    if (!f) { printf("HATA: %s acilamadi!\n", dosya); return; }

    /* Baslik satiri (Kaggle formatiyla uyumlu) */
    fprintf(f, "id,name,album,album_id,artists,artist_ids,track_number,"
               "disc_number,explicit,danceability,energy,key,loudness,"
               "mode,speechiness,acousticness,instrumentalness,liveness,"
               "valence,tempo,duration_ms,time_signature,year,release_date\n");

    int sanatci_sayisi = sizeof(SANATCILAR) / sizeof(SANATCILAR[0]);
    int album_sayisi   = sizeof(ALBUMLER)   / sizeof(ALBUMLER[0]);

    for (int i = 0; i < n; i++) {
        int sure_ms = (120 + rand() % 240) * 1000;
        int yil     = 1990 + rand() % 35;
        fprintf(f, "ID_%05d,Track_%05d,%s,ALB_%05d,%s,ART_%05d,"
                   "1,1,0,0.5,0.7,0,0.0,1,0.05,0.1,0.0,0.1,0.5,120.0,"
                   "%d,4,%d,%d-01-01\n",
                i, i,
                ALBUMLER[rand() % album_sayisi],
                rand() % 1000,
                SANATCILAR[rand() % sanatci_sayisi],
                rand() % 500,
                sure_ms,
                yil, yil);
    }
    fclose(f);
    printf("%d satir uretildi -> %s\n", n, dosya);
}

int main(int argc, char* argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 100000;
    veri_uret("data/sarkilar.csv", n);
    return 0;
}
