#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sbgCom.h>
#include <sys/time.h>

int i = 0;

#define N 300

float rolling[N];
float pitch[N];
float yaw[N];

float acc_x[N];
float acc_y[N];
float acc_z[N];

float delta_x[N];
float delta_y[N];
float delta_z[N];

float Rxx[N];
float Rxy[N];
float Rxz[N];
float Ryx[N];
float Ryy[N];
float Ryz[N];
float Rzx[N];
float Rzy[N];
float Rzz[N];


unsigned long long int instant[N];
struct timeval crtTime;


// Fonction de handler qui décrit ce qu'il faut faire quand on reçois une erreur
void continuousErrorCallback(SbgProtocolHandleInt *pHandler, SbgErrorCode errorCode, void *pUsrArg)
{
	char errorMsg[256];

	// Conversion du code d'erreur en string pour que l'utilisateur puisse le lire
	sbgComErrorToString(errorCode, errorMsg);

	// Affichage du message d'erreur
	fprintf(stderr,"continuousErrorCallback: We have received the following error %s\n", errorMsg);

	// Vidage du tampon d'erreur
	fflush(stderr);
}


 // Fonction handler qui décrit ce qu'il faut faire lors ce qu'on reçoit une trame continue valide
void continuousCallback(SbgProtocolHandleInt *handler, SbgOutput *pOutput, void *pUsrArg)
{
	if (pOutput) // Vérification qu'on a bien des sorties
	{

        // Stockage des angles dans des tableaux
        rolling[i] = SBG_RAD_TO_DEG(pOutput->stateEuler[0]);
        pitch[i] = SBG_RAD_TO_DEG(pOutput->stateEuler[1]);
        yaw[i] = SBG_RAD_TO_DEG(pOutput->stateEuler[2]);
        acc_x[i] = pOutput->accelerometers[0];
        acc_y[i] = pOutput->accelerometers[1];
        acc_z[i] = pOutput->accelerometers[2];
        delta_x[i] = pOutput->deltaAngles[0];
        delta_y[i] = pOutput->deltaAngles[1];
        delta_z[i] = pOutput->deltaAngles[2];
        Rxx[i] = pOutput->stateMatrix[0];
        Ryx[i] = pOutput->stateMatrix[1];
        Rzx[i] = pOutput->stateMatrix[2];
        Rxy[i] = pOutput->stateMatrix[3];
        Ryy[i] = pOutput->stateMatrix[4];
        Rzy[i] = pOutput->stateMatrix[5];
        Rxz[i] = pOutput->stateMatrix[6];
        Ryz[i] = pOutput->stateMatrix[7];
        Rzz[i] = pOutput->stateMatrix[8];



        // Récupération des instants de mesure
        gettimeofday(&crtTime,0);

        // Stockage des instants dans un tableau
        instant[i] = crtTime.tv_sec*1e6+crtTime.tv_usec;
        i=i+1;

	}
}


int main(int argc, char** argv)
{
    SbgProtocolHandle protocolHandle;
    SbgErrorCode error;
    SbgErrorCode error_2;

    int reponse = 0;

    if(sbgComInit("/dev/ttyUSB0",460800, &protocolHandle)==SBG_NO_ERROR)
        {
        sbgSetLogicOutChannel(protocolHandle,0,SBG_OUT_DISABLED,SBG_OUT_RISING_EDGE,10);
        sbgProtocolClose(protocolHandle);
        }

    reponse = scanf("%d",&reponse);

    if(reponse == 1)
    {
        // Initialisation de la communication.
        if(sbgComInit("/dev/ttyUSB0",460800, &protocolHandle)==SBG_NO_ERROR)
        {
            sbgSleep(20);
            printf("Initialisation OK \n");


            // Activation du mode continu et vérification
            // Le dernier argument est le diviseur de fréquence (fréquence initiale = 100 Hz)
            error = sbgSetContinuousMode(protocolHandle, SBG_CONTINUOUS_MODE_ENABLE,20);

            if(error != SBG_NO_ERROR)
            {
                printf("Echec de l'activation du mode Continu");
            }


            // Donne le masque par défaut pour le mode continu afin de choisir la sortie voulue
            error_2 = sbgSetDefaultOutputMask(protocolHandle,SBG_OUTPUT_EULER | SBG_OUTPUT_ACCELEROMETERS | SBG_OUTPUT_DELTA_ANGLES | SBG_OUTPUT_MATRIX); // Pour changer de sortie, changez le masque

            if(error_2 != SBG_NO_ERROR)
            {
                printf("Echec de la mise en place du masque de sortie par défaut");
            }


            // Définition des fonctions d'appel des handler
            sbgSetContinuousErrorCallback(protocolHandle, continuousErrorCallback, NULL);
            sbgSetContinuousModeCallback(protocolHandle, continuousCallback, NULL);

            // Envoie d'un signal de synchronisation
            // La longueur du signal logique de synchronisation est donnée en modifiant le dernier argument.
            sbgSetLogicOutChannel(protocolHandle,0,SBG_OUT_MAIN_LOOP_DIVIDER,SBG_OUT_RISING_EDGE,10);


            while (i<=N)
            {
                // Lancement du système de protocol de scan du port série
                sbgProtocolContinuousModeHandle(protocolHandle);

            }


            // Fermeture du système de protocol et libération de la mémoire associée
            sbgProtocolClose(protocolHandle);


            // Création d'un fichier texte contenant les angles
            FILE* angles = NULL;
            angles = fopen("angles.txt", "w+");
            if(angles != NULL)
            {
                int k;
                for(k=0;k<=N;k++)
                {
                    // Ecriture dans le fichier texte
                    fprintf(angles,"%3.6f\t%3.6f\t%3.6f \n",rolling[k],pitch[k],yaw[k]);
                }
                fclose(angles);
            }

            // Création d'un fichier texte contenant les accélérations
            FILE* acc = NULL;
            acc = fopen("accelerations.txt", "w+");
            if(acc != NULL)
            {
                int k;
                for(k=0;k<=N;k++)
                {
                    // Ecriture dans le fichier texte
                    fprintf(acc,"%3.6f\t%3.6f\t%3.6f \n",acc_x[k],acc_y[k],acc_z[k]);
                }
                fclose(acc);
            }

            // Création d'un fichier texte contenant les instants de mesure
            FILE* tempo = NULL;
            tempo = fopen("instants.txt", "w+");
            if(tempo != NULL)
            {
                int k;
                for(k=0;k<=N;k++)
                {
                    // Ecriture dans le fichier texte
                    fprintf(tempo,"%lli \n",instant[k]);
                }
                fclose(tempo);
            }

            FILE* delta = NULL;
            delta = fopen("delta_angles.txt", "w+");
            if(delta != NULL)
            {
                int k;
                for(k=0;k<=N;k++)
                {
                    // Ecriture dans le fichier texte
                    fprintf(delta,"%3.6f\t%3.6f\t%3.6f \n",delta_x[k],delta_y[k],delta_z[k]);
                }
                fclose(delta);
            }

            FILE* matrice = NULL;
            matrice = fopen("matrice_rotation_centrale.txt", "w+");
            if(matrice != NULL)
            {
                int k;
                for(k=0;k<=N;k++)
                {
                    // Ecriture dans le fichier texte
                    fprintf(matrice,"%3.6f\t%3.6f\t%3.6f\t%3.6f\t%3.6f\t%3.6f\t%3.6f\t%3.6f\t%3.6f\n",Rxx[k],Rxy[k],Rxz[k],
                                                                                                        Ryx[k],Ryy[k],Ryz[k],
                                                                                                        Rzx[k],Ryz[k],Rzz[k]);
                }
                fclose(matrice);
            }


        return 0;

        }
        else // En cas d'échec de l'initialisation
        {
            fprintf(stderr, "Unable to open IG-500 device\n");
            fflush(stderr);
            return -1;
        }
    }

    return 0;
}
