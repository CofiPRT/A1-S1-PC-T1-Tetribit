#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <windows.h>
#include <math.h>

void afisare(uint64_t harta, int linii_complete, int gameover)
{
    //gameover 1 pentru afisarea finala (cu scor)
	int i, j = 1, nr_zerouri = 0; //nr_zerouri pentru afisarea finala
	for (i = 63; i >= 0; i--)
	{
	    uint64_t tester = (1llu << i);
	    //pentru a testa daca bitul i+1 din "harta" este setat
	    //"ull" pentru a evita conversia automata la 32bit (cauzata de i)
		if (harta & tester)
		{
			printf("#");
		}
		else
		{
			printf(".");
			if (gameover)
            {
                nr_zerouri++;
            }
		}
		//folosim un j pentru a tine cont de lungimea randului (8 caractere)
		if (j % 8 == 0)
		{
			printf("\n");
		}
		j++;
	}
	printf("\n");

    if (gameover)
    {
        float scor = sqrt(nr_zerouri) + pow(1.25, linii_complete);
        printf("GAME OVER!\nScore:%.2f", scor);
    }
    //Sleep(1000);
}

int col_ver(uint64_t harta, int piesa, int inaltime)
{
    if (inaltime == 0)
    {
        //daca piesa se afla la baza hartii, se va opri
        return 1;
    }
    //comparam cei 8 biti de "sub" piesa cu bitii piesei
    uint16_t biti = (harta >> 8*(inaltime - 1)) & piesa;
    if (biti)
    {
        /*daca rezultatul operatiei "&" este diferit de 0,
        inseamna ca va avea loc o coliziune*/
        return 1;
    }
    return 0;
}

int col_lat(uint64_t harta, int piesa, int inaltime, int directie)
{
    /*returneaza cate transformari va executa piesa
    intr-o anumita directie*/
    if (directie == 0)
    {
        /*daca nu se deplaseaza in nicio directie
        nu are rost sa mai testam coliziuni laterale*/
        return 0;
    }
    int i;
    uint64_t biti;
    int nrmutari = 0; //numarul maxim de transformari

    for (i = 0; i < 8; i++)
    {
        biti = (harta >> 8*inaltime) & piesa;
        if (biti)
        {
            /*daca piesa se suprapune peste un obstacol lateral,
            inseamna ca am facut un pas prea mult, deci i-1 este
            numarul maxim de transformari in acea directie*/
            nrmutari = i-1;
            break;
        }

        if (((piesa & 257) && directie>0) || ((piesa & 32896) && directie<0))
        {
            /*daca s-a ajuns la o margine iar transformarea se face
            in directia celei mai apropiate margini, i este numarul
            maxim de transformari in acea directie*/
            nrmutari = i;
            break;
        }

        if (directie > 0)
        {
            piesa >>= 1;
        }
        if (directie < 0)
        {
            piesa <<= 1;
        }
    }
    /*acum returnam cea mai mica valoare dintre nr de
    transformari dorit si numarul maxim de transformari
    (in directia corespunzatoare)*/
    if (directie >= nrmutari)
    {
        return nrmutari;
    }
    if (directie > 0 && directie < nrmutari)
    {
        return directie;
    }
    if (directie < 0 && directie*(-1) >= nrmutari)
    {
        return nrmutari;
    }
    if (directie < 0 && directie*(-1) < nrmutari)
    {
        return directie*(-1);
    }
    return 0;
}

uint64_t* testlinie(uint64_t valori[])
{
    //pentru eliminarea liniilor complete
    int i;
    /*pe prima pozitie vom salva harta iar
    pe a doua vom salva liniile completate*/
    int linii_complete = 0;
    uint64_t harta_aux, harta_aux2;
    /*2 variabile pentru a evita calculele lungi
    (precum cele de 80 de caractere pe linie)*/
    uint64_t harta = valori[0];
    for (i = 0; i < 8; i++)
    {
        if ((255 & (harta >> 8*i)) == 255)
        {
            linii_complete++;
                //printf("i = %d\n", i);
                //printf("harta:\n");
                //afisare(harta, 0, 0);
            /*"turtim" harta originala astfel incat sa ramanem
            doar cu liniile de sub linia completata*/
            if (i)
            {
                harta_aux = harta << (8*(8 - i));
                harta_aux >>= 8*(8 - i);
            }
            else
            {
                /*pentru i=0 => 8*(8-i)=64. Din moment ce nu putem "shifta"
                o variabila uint64_t de 64 de ori sau mai mult, atribuim
                simplu valoarea 0*/
                harta_aux = 0;
            }
                //printf("harta_aux:\n");
                //afisare(harta_aux, 0, 0);
            //"turtim" partea hartii de deasupra liniei completate
            if (i == 7)
            {
                /*pentru i=7 => 8*(i+1)=64. Aceeasi explicatie precum
                cea antepusa*/
                harta_aux2 = 0;
            }
            else
            {
                harta_aux2 = harta >> (8*(i + 1));
                harta_aux2 <<= 8*i;
            }

                //printf("harta_aux2:\n");
                //afisare(harta_aux2, 0, 0);
            /*"lipim" cele doua parti de harta (de
            sub si respectiv deasupra liniei completate*/
            harta = harta_aux | harta_aux2;
                //printf("harta_harta:\n");
                //afisare(harta, 0, 0);
            /*mergem un pas inapoi pentru a verifica daca s-au completat
            mai multe linii in acelasi timp*/
            i--;
        }
    }
    valori[0] = harta;
    valori[1] = linii_complete;
    return valori;
}

int main()
{
	uint64_t harta;
	int nr_piese = 0;

	scanf("%" SCNu64, &harta);
	scanf("%d", &nr_piese);

	int transf[nr_piese][9]; //9 = piesa + 8 transformari

	int i, j; //citirea pieselor si a transformarilor
    for (i = 0; i < nr_piese; i++)
    {
        for (j = 0; j < 9; j++)
        {
            scanf("%d", &transf[i][j]);
        }
    }

    if (nr_piese == 0)
    {
        /*daca nu sunt piese cu care sa se lucreze
        jocul se termina*/
        afisare(harta, 0, 1);
    }
    else
    {
        afisare(harta, 0, 0);
    }

    int inaltime, linii_complete = 0, piesa;
    //inaltime 0 este prima linie de jos a hartii
    uint64_t harta_af;
    /*harta_af utilizata pentru afisari*/
    uint64_t *pointer, valori[3];
    /*pentru functia testlinie:
    valori[0] salveaza harta iar valori[1] valoarea linii_complete*/
    for (i = 0; i < nr_piese; i++)
    {
        inaltime = 8;
        piesa = transf[i][0];
        printf("%d\n", piesa);
        for (j = 1; j <= 9; j++)
        {
            if (col_ver(harta, piesa, inaltime))
            {
                //daca va avea loc o coliziune
                if (inaltime == 8)
                {
                    /*daca piesa nu a intrat deloc in harta
                    (nu are loc), jocul se termina*/
                    afisare(harta, linii_complete, 1);
                    i = nr_piese;
                    break;
                }
                if (inaltime == 7 && piesa >= 256)
                {
                    /*daca o piesa de inaltime 2 a intrat pe jumatate
                    in harta si nu poate continua, jocul se termina*/
                    harta = harta | ((uint64_t)piesa << 8*inaltime);
                    afisare(harta, linii_complete, 1);
                    i = nr_piese;
                    break;
                }
                //salvam noua harta si trecem la urmatoarea piesa
                harta = harta | ((uint64_t)piesa << 8*inaltime);

                if (i + 1 == nr_piese)
                {
                    //daca aceasta a fost ultima piesa, jocul se termina
                    afisare(harta, linii_complete, 1);
                }
                break;
            }

            /*daca nu va avea loc o coliziune, coboram
            piesa si executam transformarea*/
            inaltime--;

            if (transf[i][j] >= 0)
            {
                piesa >>= col_lat(harta, piesa, inaltime, transf[i][j]);
            }
            if (transf[i][j] < 0)
            {
                piesa <<= col_lat(harta, piesa, inaltime, transf[i][j]);
            }

            harta_af = harta | ((uint64_t)piesa << 8*inaltime);

            if (col_ver(harta, piesa, inaltime))
            {
                //daca piesa nu mai poate continua
                harta = harta_af;
                //salvam harta pentru a trece la urmatoarea piesa

                valori[0] = harta_af;
                valori[1] = linii_complete;
                pointer = testlinie(valori);

                if (harta_af != pointer[0])
                {
                    //daca s-au eliminat linii, scorul creste
                    linii_complete += pointer[1];
                    afisare(harta_af, linii_complete, 0);
                    /*afisam harta inainte de eliminarea liniilor
                    dupa care o salvam pentru afisari ulterioare*/
                    harta = pointer[0];
                    harta_af = harta;
                }
                if (i + 1 == nr_piese)
                {
                    //daca s-a ajuns la ultima piesa, jocul se termina
                    afisare(harta_af, linii_complete, 1);
                    break;
                }

                afisare(harta_af, linii_complete, 0);
                break;
            }
            afisare(harta_af, linii_complete, 0);
        }
    }
	return 0;
}
