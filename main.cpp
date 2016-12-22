#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdlib.h>   
#include <time.h>
#include <random>

using namespace std;

const string KReset("0");
const string KNoir("30");
const string KRouge("31");
const string KVert("32");
const string KJaune("33");
const string KBleu("34");
const string KMagenta("35");
const string KCyan("36");

typedef vector <char> CVLine;
typedef vector <CVLine> CMatrix;


typedef struct {
	unsigned m_X;
	unsigned m_Y;
	unsigned m_sizeX;
	unsigned m_sizeY;
	char m_token;
} SPlayer;

typedef struct {
	unsigned m_X;
	unsigned m_Y;
	char m_token;
} SObstacle;

typedef struct {
	unsigned m_X;
	unsigned m_Y;
	unsigned m_sizeX;
	unsigned m_sizeY;
	char m_token;
} SBonus;

namespace {
	/* VARIABLES DE CONFIGURATION */

	vector <SObstacle> VObstacle;
	vector <string> VOptionsName;
	vector <string> VOptionValue;


	unsigned KSizeY(10);
	unsigned KSizeX(10);

	char KBonus;
	char KLeft;
	char KTop;
	char KBot;
	char KRight;
	char KEmpty;
	char KObstacle;
	char KBorderLine;
	char KBorderColumn;
	char KFirstPlayer;
	char KSecondPlayer;


	//AUTRE 

	void Couleur(const string & coul) {
		cout << "\033[" << coul << "m";

	}

	void ClearScreen() {
		cout << "\033[H\033[2J";
	}

	int IntRandom(int min, int max) { // Taken from Stackoverflow
		random_device randm;
		mt19937 rng(randm());
		uniform_int_distribution<int> uni(min, max);

		return uni(rng);
	}

	unsigned AskTourMax() {
		unsigned nbrnds;
		cout << "Entrez le nombre de rounds" << endl;
		cin >> nbrnds;
		return nbrnds;
	}

	// OPTIONS

	void InitOptions() {

		//All configs go here !AND! in header part !AND! in SetConfig();

		KBonus = 'B';
		KLeft = 'q';
		KTop = 'z';
		KBot = 's';
		KRight = 'd';
		KEmpty = '.';
		KObstacle = '#';
		KBorderLine = '#';
		KBorderColumn = '#';
		KFirstPlayer = 'X';
		KSecondPlayer = 'O';

		VOptionsName = { "KBonus", "KLeft", "KTop", "KBot", "KRight", "KEmpty", "KObstacle", "KBorderLine", "KBorderColumn", "KFirstPlayer", "KSecondPlayer" };
		VOptionValue = { "B", "q", "z", "s" ,"d ",".", "#" ,"#","#", "X" ,"O" }; //Valeurs pour VOptionsName[i]


	}

	void SetConfig(string Name, const char Value) {

		for (unsigned i(0); i < VOptionsName.size(); ++i)
			if (VOptionsName[i] == Name) VOptionValue[i] = Value;


		if (Name == "KLeft") KLeft = Value;

		else if ("KTop" == Name) KTop = Value;

		else if ("KBot" == Name) KBot = Value;

		else if ("KRight" == Name) KRight = Value;

		else if ("KObstacle" == Name) KObstacle = Value;

		else if ("KBorderLine" == Name) KBorderLine = Value;

		else if ("KBorderColumn" == Name) KBorderColumn = Value;

		else if ("KFirstPlayer" == Name) KFirstPlayer = Value;

		else if ("KSecondPlayer" == Name) KSecondPlayer = Value;

		/*if ((!(KBorderLine == KBorderColumn)) || (!(KBorderLine == KObstacle)) || (!(KBorderColumn == KObstacle))) {
			Couleur(KRouge);
			cout << "[!] KBorderLine, KObstacle et KBorderColumn doivent avoir le même caractère" << endl;
			Couleur(KReset);
		}  // Semble ne peut être dérangeant ? */
	}
	// MATRICE

	CMatrix InitMat(unsigned NbLine, unsigned NbColumn, SPlayer & FirstPlayer, SPlayer & SecondPlayer, bool ShowBorder = true) {
		CMatrix tmpMatrix;
		tmpMatrix.resize(NbLine);

		for (unsigned i(0); i < NbLine; ++i)
			for (unsigned j(0); j < NbColumn; ++j)
				tmpMatrix[i].push_back(KEmpty);



		for (unsigned i(FirstPlayer.m_Y); i < FirstPlayer.m_Y + FirstPlayer.m_sizeY; ++i)
			for (unsigned j(FirstPlayer.m_X); j < FirstPlayer.m_X + FirstPlayer.m_sizeX; ++j)
				tmpMatrix[i][j] = FirstPlayer.m_token;


		for (unsigned i(SecondPlayer.m_Y); i < SecondPlayer.m_Y + SecondPlayer.m_sizeY; ++i)
			for (unsigned j(SecondPlayer.m_X); j < SecondPlayer.m_X + SecondPlayer.m_sizeX; ++j)
				tmpMatrix[i][j] = SecondPlayer.m_token;

		if (ShowBorder) {
			for (unsigned i(0); i < NbLine; ++i) {

				tmpMatrix[i][0] = KBorderColumn;
				tmpMatrix[i][NbColumn - 1] = KBorderColumn;
			}

			for (unsigned i(0); i < NbColumn; ++i) {
				tmpMatrix[0][i] = KBorderLine;
				tmpMatrix[NbLine - 1][i] = KBorderLine;
			}

		}

		return tmpMatrix;
	}

	void ShowMatrix(const CMatrix & Mat) {
		ClearScreen();
		Couleur(KReset);
		for (unsigned i(0); i < Mat.size(); ++i) {
			for (unsigned a(0); a < Mat[i].size(); ++a) {

				if (Mat[i][a] == KObstacle) Couleur(KNoir);

				if (Mat[i][a] == KBonus) Couleur(KVert);

				if (Mat[i][a] == KFirstPlayer) Couleur(KRouge);

				if (Mat[i][a] == KSecondPlayer)	Couleur(KBleu);

				cout << Mat[i][a];
				Couleur(KReset);

			}
			cout << endl;
		}
	}

	// WIN CHECK - WIN STAT

	bool CheckIfWin(SPlayer & FirstPlayer, SPlayer & SecondPlayer) {
		return !((FirstPlayer.m_X > SecondPlayer.m_X + SecondPlayer.m_sizeX - 1) ||
			(FirstPlayer.m_X + FirstPlayer.m_sizeX - 1 < SecondPlayer.m_X) ||
			(SecondPlayer.m_Y > FirstPlayer.m_Y + FirstPlayer.m_sizeY - 1) ||
			(SecondPlayer.m_Y + SecondPlayer.m_sizeY - 1 < FirstPlayer.m_Y));
	}

	char GetWinner(SPlayer& FirstPlayer, SPlayer &SecondPlayer, const unsigned & NbrTour) {
		return (NbrTour % 2 == 0 ? FirstPlayer.m_token : SecondPlayer.m_token);
	}


	// BONUS

	SBonus InitBonus(const unsigned largeur, const unsigned hauteur, const unsigned
		AxeX, const unsigned AxeY, const char Token) {

		SBonus tmpBonus;

		tmpBonus.m_sizeX = largeur;
		tmpBonus.m_sizeY = hauteur;
		tmpBonus.m_X = AxeX;
		tmpBonus.m_Y = AxeY;
		tmpBonus.m_token = Token;

		return tmpBonus;
	}

	void PutBonus(CMatrix & Matrice, SBonus & Bonus) {
		Matrice[Bonus.m_Y][Bonus.m_X] = Bonus.m_token;
	}

	void GetBonus(CMatrix & Mat, SPlayer & Player) {
		for (unsigned i(Player.m_Y); i < Player.m_Y + Player.m_sizeY; ++i) {
			for (unsigned j(Player.m_X); j < Player.m_X + Player.m_sizeX; ++j) {
				//Début de la détéction des bonus

				if (Mat[i][j] == KBonus) {

					++Player.m_sizeX;
					++Player.m_sizeY;

				}

				//Fin de la détéction des bonus
				for (unsigned i(Player.m_Y); i < Player.m_Y + Player.m_sizeY; ++i)
					for (unsigned j(Player.m_X); j < Player.m_X + Player.m_sizeX; ++j)
						Mat[i][j] = Player.m_token;
			}
		}
	}


	// OBSTACLES

	SObstacle InitObstacle(const unsigned
		AxeX, const unsigned AxeY, const char Token) {

		SObstacle Obstacle;

		Obstacle.m_X = AxeX;
		Obstacle.m_Y = AxeY;
		Obstacle.m_token = Token;
		return Obstacle;
	}

	void PutObstacle(CMatrix & Matrice, SObstacle & obstacle) {
		VObstacle.push_back(obstacle);

		Matrice[obstacle.m_Y][obstacle.m_X] = obstacle.m_token;

	}

	void GenerateRandomObstacles(CMatrix & Matrice, const SObstacle & Obstacle, const unsigned & SizeObs) {
		SObstacle tmpObs = Obstacle;

		vector <unsigned> randomValues;

		for (unsigned i(0); i < SizeObs; ++i) randomValues.push_back(IntRandom(1, 2));


		for (unsigned i(0); i < SizeObs; ++i) {

			switch (randomValues[i])
			{
			case 1:
				++tmpObs.m_Y;
				PutObstacle(Matrice, tmpObs);

			case 2:
				++tmpObs.m_X;
				PutObstacle(Matrice, tmpObs);

			}
		}
	}

	bool IsMovementAllowed(const SPlayer & Player, char & Movement) {

		for (unsigned i(0); i < VObstacle.size(); ++i) {
			if (VObstacle[i].m_X == Player.m_X  &&
				VObstacle[i].m_Y == Player.m_Y - 1 && Movement == KTop) return true;
			else if (VObstacle[i].m_X == Player.m_X  &&
				VObstacle[i].m_Y == Player.m_Y + 1 && Movement == KBot) return true;

			else if (VObstacle[i].m_X == Player.m_X - 1 &&
				VObstacle[i].m_Y == Player.m_Y  && Movement == KLeft) return true;

			else if (VObstacle[i].m_X == Player.m_X + 1 &&
				VObstacle[i].m_Y == Player.m_Y  && Movement == KRight) return true;
		}
		return false;
	}

	// PLAYERS

	SPlayer InitPlayer(const unsigned  largeur, const unsigned  hauteur, const unsigned  AxeX, const unsigned  AxeY, const char  Token) {

		SPlayer tmpPlayer;

		tmpPlayer.m_sizeX = largeur;
		tmpPlayer.m_sizeY = hauteur;
		tmpPlayer.m_X = AxeX;
		tmpPlayer.m_Y = AxeY;
		tmpPlayer.m_token = Token;

		return tmpPlayer;
	}

	void MovePlayer(CMatrix & Mat, char Move, SPlayer & player) {

		if (IsMovementAllowed(player, Move)) return;


		if (Move == KTop) {
			if (player.m_Y > 1) //Verifie les bordures
			{

				player.m_Y -= 1;

				GetBonus(Mat, player);
				for (unsigned i(player.m_X); i < player.m_X + player.m_sizeX; ++i) {
					Mat[player.m_Y + player.m_sizeY][i] = KEmpty;
					Mat[player.m_Y][i] = player.m_token;

				}
			}
		}


		else if (Move == KBot) {

			if (player.m_Y + player.m_sizeY < Mat.size() - 1) {
				player.m_Y += 1;
				GetBonus(Mat, player);

				for (unsigned i(player.m_X); i < player.m_X + player.m_sizeX; ++i) {
					Mat[player.m_Y - 1][i] = KEmpty;
					Mat[player.m_Y + player.m_sizeY - 1][i] = player.m_token;

				}
			}
		}

		else if (Move == KLeft) {
			if (player.m_X > 1)
			{
				player.m_X -= 1;
				GetBonus(Mat, player);

				for (unsigned i(player.m_Y); i < player.m_Y + player.m_sizeY; ++i) {
					Mat[i][player.m_X + player.m_sizeX] = KEmpty;
					Mat[i][player.m_X] = player.m_token;
				}
			}
		}

		else if (Move == KRight) {
			if (player.m_X + player.m_sizeX < Mat[0].size() - 1)
			{
				player.m_X += 1;
				GetBonus(Mat, player);

				for (unsigned i(player.m_Y); i < player.m_Y + player.m_sizeY; ++i) {
					Mat[i][player.m_X - 1] = KEmpty;
					Mat[i][player.m_X + player.m_sizeX - 1] = player.m_token;
				}
			}
		}
	}

	//DISPLAYS


	void DisplayMenu();

	void DisplayMulti() {


		unsigned NbRnds = AskTourMax();
		char EnteredKey;


		SPlayer FirstPlayer = InitPlayer(1, 1, 1, 4, KFirstPlayer);

		SPlayer SecondPlayer = InitPlayer(1, 1, 8, 8, KSecondPlayer);

		SObstacle FirstObstacle = InitObstacle(2, 5, KObstacle);

		SBonus FirstBonus = InitBonus(2, 2, 4, 4, KBonus);

		CMatrix Map = InitMat(KSizeX + 1, KSizeY + 1, FirstPlayer, SecondPlayer); // +1 due à la bordure de '#' le long de la matrice


		GenerateRandomObstacles(Map, FirstObstacle, 4);
		PutBonus(Map, FirstBonus);


		ShowMatrix(Map);

		for (unsigned i(0); i < NbRnds * 2; ++i)
		{

			ShowMatrix(Map);
			SPlayer & actualPlayer = (i % 2 == 0 ? FirstPlayer : SecondPlayer);

			cout << "Au tour de " << actualPlayer.m_token << endl;
			cin >> EnteredKey;

			MovePlayer(Map, EnteredKey, actualPlayer);

			if (CheckIfWin(FirstPlayer, SecondPlayer))
			{
				cout << GetWinner(FirstPlayer, SecondPlayer, i) << " a gagné !" << endl;
				return;
			}


		}

		cout << "Egalité !" << endl;
	}

	void DisplayOption() {

		ClearScreen();
		const vector <string> optiontitle = {
			" _____   _____   _____   _   _____   __   _   _____  ",
			"/  _  \\ |  _  \\ |_   _| | | /  _  \\ |  \\ | | /  ___/ ",
			"| | | | | |_| |   | |   | | | | | | |   \\| | | |___  ",
			"| | | | |  ___/   | |   | | | | | | | |\\   | \\___  \\ ",
			"| |_| | | |       | |   | | | |_| | | | \\  |  ___| | ",
			"\\_____/ |_|       |_|   |_| \\_____/ |_|  \\_| /_____/ ",
			"\n"
		};
		for (string Lines : optiontitle) cout << ' ' << Lines << endl;

		cout << "Liste des options par defaut : " << endl;

		for (unsigned i(0); i < VOptionsName.size(); ++i) cout << i << ". " << VOptionsName[i] << " : '" << VOptionValue[i] << '\'' << endl;

		Couleur(KRouge);

		cout << endl << "[!] Attention, ces configurations ne seront présentes jusqu'à la fermeture complète du jeu." << endl;

		Couleur(KVert);

		cout << endl << "[?] Voulez-vous modifier une option ? [Y/n] ";

		char choix;
		cin >> choix;

		if (choix == 'y') {

			cout << endl << "[+] Afin de modifier un paramètre, veuillez entrer le numéro correspondant : ";
			unsigned numero;
			cin >> numero;
			cout << VOptionsName[numero] << " deviendra : ";
			char newparam;
			cin >> newparam;
			SetConfig(VOptionsName[numero], newparam);

			Couleur(KReset);

		}
		else if (choix == 'n') exit(0);

		DisplayMenu();

		Couleur(KReset);
	}

	void DisplaySolo() {

		//TODO

	}

	void DisplayMenu() {
		ClearScreen();

		vector <string> MenuList = { "Singleplayer (IA)", "Multiplayer", "Options", "Exit" };

		unsigned choice(0);

		const vector <string>  menutitle = {
			"     ___  ___   _____   __   _   _   _  ",
			"    /   |/   | | ____| |  \\ | | | | | | ",
			"   / /|   /| | | |__   |   \\| | | | | | ",
			"  / / |__/ | | |  __|  | |\\   | | | | | ",
			" / /       | | | |___  | | \\  | | |_| | ",
			"/_/        |_| |_____| |_|  \\_| \\_____/ ",
			"\n"
		};

		for (string Lines : menutitle) cout << ' ' << Lines << endl;

		for (string Option : MenuList) {
			++choice;

			cout << ' ';
			Couleur(KRouge);
			cout << "[" << choice << "] ";
			Couleur(KReset);
			cout << Option << endl << endl;

		}
		cout << endl << "Choose between [1-" << MenuList.size() << "] : ";

		unsigned input;
		cin >> input;

		switch (input) {
		case 1:
			DisplaySolo();
			break;
		case 2:
			DisplayMulti();
			break;

		case 3:
			DisplayOption();
			break;

		case 4:
			exit(0);

		default:

			Couleur(KRouge);
			cout << "[!] Choix invalide !" << endl;
			Couleur(KReset);
		}
	}

}

int main()
{
	InitOptions();
	DisplayMenu();
	return 0;
}
