#include <iostream>
#include <array>
#include <fstream>
#include <istream>
#include <string>
#include <ctime>

const int nb_types_depenses {4};

const std::string depenses_file_name {"depenses.txt"};
const std::string bonus_file_name {"bonus_malus.txt"};

using tab_depenses = std::array<float, nb_types_depenses>;
const tab_depenses depenses_semaine {20, 33.25, 3, 5}; //les dépenses dans l'ordre : ru, bouffe normale, divers
const tab_depenses depenses_mois {66, 133, 12, 20}; //idem qu'au dessus mais pour le mois

bool new_week(std::string file_name){
    std::ifstream file_dernier_jour("dernier_jour.txt");
    time_t actuel = time(0);
    int dernier_jour;

    tm *ltm = localtime(&actuel);

    file_dernier_jour >> dernier_jour;

    if (dernier_jour > ltm -> tm_wday){
        std::ofstream file_dernier_jour_w("dernier_jour.txt");
        file_dernier_jour_w << ltm -> tm_wday;
        return true;
    }
    else{
        std::ofstream file_dernier_jour_w("dernier_jour.txt");
        file_dernier_jour_w << ltm -> tm_wday;
        return false;
    }
}

int nb_repas_restant(){
    time_t actuel = time(0);
    tm *ltm = localtime(&actuel);

    return 9 - (ltm -> tm_wday-1);
}

tab_depenses depenses_totales_semaine(){
    std::ifstream depenses_file(depenses_file_name);
    float prix;
    int i = 0;
    tab_depenses result {0, 0, 0, 0};

    while (depenses_file >> prix){
        result[i] += prix;
        i++;
        i = i%4;
    }
    return result;
}

tab_depenses argent_bonus(std::string file_name){
    tab_depenses result {0, 0, 0, 0};
    std::ifstream file(file_name);

    for (int i = 0; i < nb_types_depenses; i++){
        file >> result[i];
    }

    return result;
}

void reset(std::string file_name){
    std::ofstream file(file_name);
    file << "";
    file.close();
}

void write_expenses(tab_depenses expenses, std::string file_name){
    std::ofstream file(file_name, std::ios::app);

    if (file){
        for (int i = 0; i < expenses.size(); i++){
            file << expenses[i] << " ";
        }
        file << std::endl;
    }
}

tab_depenses argent_restant_semaine(){
    tab_depenses dep_totales = depenses_totales_semaine();
    tab_depenses result;

    for (int i = 0; i < result.size(); i++){
        result[i] = depenses_semaine[i] - dep_totales[i];
    }

    return result;
}

void update_bonus_file(){
    tab_depenses argent_restant = argent_restant_semaine();
    tab_depenses argent_bonus_restant = argent_bonus(bonus_file_name);
    tab_depenses new_argent_bonus;
    std::ofstream bonus_file(bonus_file_name);

    for (int i = 0; i < nb_types_depenses; i++){
        new_argent_bonus[i] = argent_bonus_restant[i] + argent_restant[i];
        bonus_file << new_argent_bonus[i];
        bonus_file << " ";
    }
}

void write_bonus_malus(tab_depenses writing_values){
    std::ofstream bonus_malus_file("bonus_malus.txt");

    for (int i = 0; i < nb_types_depenses; i++){
            bonus_malus_file << writing_values[i];
            bonus_malus_file << " ";
    }

}

void affichage_budget(tab_depenses& argent_restant, int repas_restants){
    bool est_nul;
    tab_depenses var_argent_bonus = argent_bonus("bonus_malus.txt");
    argent_restant = argent_restant_semaine();
    std::cout << "     RU      |    bouffe     |   lessive    |   autres     |" << std::endl;
    for (int i = 0; i < argent_restant.size(); i++){
        if (argent_restant[i] < 0) std::cout << "\033[31m"; // si on est dans le négatif on affiche le texte en rouge
        else std::cout << "\033[39m"; // sinon on réinitialise la couleur du texte
        printf("%.2f€", argent_restant[i]);
        if (var_argent_bonus[i] > 0){
            std::cout << "\033[32m"; // si on est dans le négatif on affiche le texte en rouge
            if ((i == 2 or i == 3) and var_argent_bonus[i] >= 10) printf("+%.2f |  ", var_argent_bonus[i]);
            if ((i == 2 or i == 3) and var_argent_bonus[i] < 10) printf("+%.2f  |  ", var_argent_bonus[i]);
        }
        else if (var_argent_bonus[i] < 0){
            std::cout << "\033[31m"; // si on est dans le négatif on affiche le texte en rouge
            if ((i == 2 or i == 3) and var_argent_bonus[i] <= -10) printf("%.2f |  ", var_argent_bonus[i]);
            if ((i == 2 or i == 3) and var_argent_bonus[i] > -10) printf("%.2f  |  ", var_argent_bonus[i]);
        }                            
        else{
            if ((i == 0 or i == 1) and argent_restant[i] >= 10) printf("       |  ");
            else if ((i == 0 or i == 1) and argent_restant[i] < 10) printf("        |  ");
            else printf("       |  ");
        }                 
        std::cout << "\033[39m"; // sinon on réinitialise la couleur du texte
    }
    printf("\n              %.2f€ (%i repas)\n\n", (argent_restant[1]+var_argent_bonus[1])/repas_restants, repas_restants);
}

void menu(){
    char choix_menu {' '};
    float prix;
    std::string nom_fichier_semaine {"depenses.txt"};
    tab_depenses argent_restant;
    bool new_w = new_week("dernier_jour.txt");
    int repas_restants = nb_repas_restant();
    if (new_w){
        update_bonus_file();
        reset(nom_fichier_semaine);
    }

    do{
        std::system("clear"); //envoie la commande clear dans la console
        affichage_budget(argent_restant, repas_restants);

        std::cout << "1 : ajouter des dépenses" << std::endl <<
            "2 : reinitialiser budget semaine" << std::endl <<
            "3 : reinitialiser les bonus/malus dans le budget" << std::endl <<
            "4 : retirer un repas a la semaine" << std::endl <<
            "q : quitter" << std::endl;


        std::cin >> choix_menu;
        if (choix_menu == '1'){
            std::cout << "1 : RU" << std::endl <<
            "2 : bouffe" << std::endl <<
            "3 : lessive" << std::endl <<
            "4 : autres" << std::endl;
    

            std::cin >> choix_menu;
            std::cin >> prix;

            tab_depenses expenses {0, 0, 0, 0};
            int choix_menu_nb = choix_menu - '0'; //on transforme le char en int
            tab_depenses var_argent_bonus = argent_bonus("bonus_malus.txt");
            if (var_argent_bonus[choix_menu_nb-1] > prix){
                var_argent_bonus[choix_menu_nb-1] -= prix;
                write_bonus_malus(var_argent_bonus);           
            }
            else if (var_argent_bonus[choix_menu_nb-1] > 0){
                prix -= var_argent_bonus[choix_menu_nb-1];
                var_argent_bonus[choix_menu_nb-1] = 0;
                write_bonus_malus(var_argent_bonus);
                expenses[choix_menu_nb-1] = prix;
                write_expenses(expenses, "depenses.txt");
            }
            else{
                expenses[choix_menu_nb-1] = prix;
                write_expenses(expenses, "depenses.txt");

            }

            choix_menu = ' ';
            std::system("clear"); //envoie la commande clear dans la console
        }

        
        
        else if (choix_menu == '2'){
            char verif = 'n';
            std::cout << "êtes vous sur ??? (o : oui)" << std::endl;
            std::cin >> verif;
            if (verif == 'o') reset(nom_fichier_semaine); 
        }
        else if (choix_menu == '3'){
            reset("bonus_malus.txt");
        }
        else if (choix_menu == '4'){
            repas_restants--; 
        }

        else if (choix_menu == '9'){
            srand((unsigned) time(NULL));
            std::array<std::string, 26> lettres {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
            std::array<std::string, 9> colors {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", "\033[37m", "\033[38m", "\033[39m"};
            std::string mot;
            std::string color = "\033[39m";
            while (true){
                color = colors[rand()%9];
                std::cout << color;
                std::cout << lettres[rand()%26];
            }
        }
    

    } while (choix_menu != 'q');
}

int main(){
    menu();

    return 0;
};
