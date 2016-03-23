////////////////////////////////////////////////////////////////////////////////
/// \file		exec.cpp
/// \author		Equipe 4359
/// \version	10 mars 2016
/// \since		10 mars 2016
///
/// Executable
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h> 
#define F_CPU 8000000
#include <util/delay.h>
#include "PWMetMinuterie.h"
#include "LEDetCapteur.h"
#include "transmissionUART.h"
#include "memoire_24.h"
#include <avr/interrupt.h>


int main ()
{
    // Branchements: PIN A1 pour bouton gauche. PIN A3 pour bouton droit. PIN A5 pour photoresistance.
    //Directions entrees (gauche/droite):
    uint8_t directions[3];  // Contient les 3 directions choisies par les boutons cliques.    
    uint8_t gauche = 0;
    uint8_t droite = 1;
    uint8_t compteur = 0;       // Compte le nombre de boutons qui ont ete cliques.
    
    while(compteur < 3)
    {
        if(PINA == 0x00){
            
            //Entree des boutons gauche droite:
            if(PINA & 0x01)     // Pese sur le bouton gauche.
            {   _delay_ms(10);
                if(PINA == 0x01)
                {
                    directions[compteur]= gauche;
                    compteur++;
                }
            }
            else if(PINA & 0x03) // Pese sur le bouton droite.
            {
                _delay_ms(10);
                if(PINA == 0x03)
                {
                    directions[compteur]= droite;
                    compteur++;
                }
            }
        }
    }
    
    DDRC = 0b00100000; // PORT C est en mode entree sauf PIN6 qui gere la calibration du Cytron
    PORTC = 0b00100000;	// Met le signal de calibration a 1.
    enum etat{AVANCE, TRD, TRG};
    uint8_t etats = AVANCE; 
    //etat = AVANCE;      // Etat initial.
    
    
    for(;;)
    {
        _delay_ms(10);
        
        // Verifie l'etat a chaque cycle de la boucle.
        if(PORTC == 0b00000100)
            etats = AVANCE;
        else if(PORTC & 0b0001000)
            etats = TRD;
        else if(PORTC & 0b00000010)
            etats = TRG;
        
        
        // Choisit l'etat.
        switch (etats) 
        {
            case (AVANCE):
                initialisation();       
                PORTD = 0b000000000;            // On avance quand PIN d3 et d4 sont 0. Ajuste donc la direction.
                if(PORTC == 0b00000100)
                {    ajustementPWM (100, 100);
                }
                break;
                
            case (TRD):
                ajustementPWM(0, 0); 
                while(PORTC & 0b0001000)
                {
                    ajustementPWM(100, 0);        // Tourne a droite jusqu'a etre droit.
                }
                break;
                
            case (TRG):
                ajustementPWM(0, 0); 
                while(PORTC & 0b0000010)
                {
                    ajustementPWM(0, 100);        // Tourne a droite jusqu'a etre droit.
                }
                break;
        }
        
    }
}