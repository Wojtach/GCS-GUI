#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	gm(GameMaster::GetInstance()),
	isPlayerInit(false),
	playerST(10),
	playerDX(10),
	playerHT(10),
	playerVet(0),
	isPlayerOutOfPoints(false),
	teamSize(0),
	currentTeam(1),
	currentTurn(0),
	currentCharacterTurn(0),
	isGameFinished(0),
	isPlayerAlive(true)
{
	ui->setupUi(this);
	ui->stackedWidget->setCurrentIndex(0);

	baddiesID[0] = -1;
	baddiesID[1] = -1;
	baddiesDead[0] = false;
	baddiesDead[1] = false;

	if (!gm.InitializeGameMaster())
	{
		this->close();
	}
	player = gm.InitBasePlayer();

	allCharacters = gm.GetCharacters();
	currentCharacterSelected = allCharacters[0];

	if (!isPlayerInit)
	{
		ui->prepareBattleButton->setEnabled(false);
		ui->prepareBattleButton->setText("Your character isn't ready!");
		ui->prepareBattleButton->adjustSize();
	}
	else
	{
		ui->prepareBattleButton->setEnabled(true);
		ui->prepareBattleButton->setText("Prepare the battle");
		ui->prepareBattleButton->adjustSize();
	}

	std::vector<Weapon> weapons = gm.GetWeapons();
	std::vector<Armour> armours = gm.GetArmours();
	std::vector<Shield> shields = gm.GetShields();
	for (auto& it : weapons)
	{
		ui->weaponComboBox->addItem(it.name.c_str());
	}
	for (auto &it : armours)
	{
		ui->armourComboBox->addItem(it.name.c_str());
	}
	for (auto &it : shields)
	{
		ui->shieldComboBox->addItem(it.name.c_str());
	}
	for (auto &it : allCharacters)
	{
		ui->comboBox->addItem(it.name.c_str());
	}

	if (currentTeam == 1)
	{
		currentTeam = 1;
		ui->errorLabel->setText("Team current size: " +
			QString::number(team1Chars.size() + 1));
	}
	else
	{
		currentTeam = 2;
		ui->errorLabel->setText("Team current size: " +
			QString::number(team2Chars.size()));
	}
}

MainWindow::~MainWindow()
{
	delete ui;
	delete player;
}

void MainWindow::on_exitButton_clicked()
{
	this->close();
}
void MainWindow::on_prepareCharacterButton_clicked()
{
	ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_prepareBattleButton_clicked()
{
	ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_pCCharacterDoneButton_clicked()
{
	if (ui->characterNameTextEdit->toPlainText().isEmpty())
	{
		QMessageBox::warning(this, "Error", "Your character name is missing!");
		return;
	}

	isPlayerInit = true;
	ui->prepareCharacterButton->setText("Edit Character");
	ui->prepareCharacterButton->adjustSize();
	ui->prepareBattleButton->setEnabled(true);
	ui->prepareBattleButton->setText("Prepare the battle");
	ui->prepareBattleButton->adjustSize();

	player->CalculateExtraAttributes();
	player->CalculateSkillsDefaults();

	ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pCCancelButton_clicked()
{
	playerST = 10;
	ui->STSpinBox->setValue(10);
	playerHT = 10;
	ui->HTSpinBox->setValue(10);
	playerDX = 10;
	ui->DXSpinBox->setValue(10);
	playerVet = 0;
	ui->VetSpinBox->setValue(0);

	delete player;
	player = gm.InitBasePlayer();
	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_characterNameTextEdit_textChanged()
{
	player->name = ui->characterNameTextEdit->toPlainText().toLocal8Bit().constData();
}

void MainWindow::on_STSpinBox_valueChanged(int i)
{
	if (isPlayerOutOfPoints)
		return;
	bool playerStillHasPoints = false;
	if (playerST > i)
	{
		playerStillHasPoints = player->ModifyAttribute(-1, 'S');
	}
	else if (playerST < i)
	{
		playerStillHasPoints = player->ModifyAttribute(1, 'S');
	}
	if (!playerStillHasPoints)
		isPlayerOutOfPoints = true;
	else
		isPlayerOutOfPoints = false;
	playerST = i;
	ui->pointsLeftLabel->setText(QString::number(player->characterPoints));
}
void MainWindow::on_DXSpinBox_valueChanged(int i)
{
	if (isPlayerOutOfPoints)
		return;
	bool playerStillHasPoints = false;
	if (playerDX > i)
	{
		playerStillHasPoints = player->ModifyAttribute(-1, 'D');
	}
	else if (playerDX < i)
	{
		playerStillHasPoints = player->ModifyAttribute(1, 'D');
	}
	if (!playerStillHasPoints)
		isPlayerOutOfPoints = true;
	else
		isPlayerOutOfPoints = false;
	playerDX = i;
	ui->pointsLeftLabel->setText(QString::number(player->characterPoints));
}
void MainWindow::on_HTSpinBox_valueChanged(int i)
{
	if (isPlayerOutOfPoints)
		return;
	bool playerStillHasPoints = false;
	if (playerHT > i)
	{
		playerStillHasPoints = player->ModifyAttribute(-1, 'H');
	}
	else if (playerHT < i)
	{
		playerStillHasPoints = player->ModifyAttribute(1, 'H');
	}
	if (!playerStillHasPoints)
		isPlayerOutOfPoints = true;
	else
		isPlayerOutOfPoints = false;
	playerHT = i;
	ui->pointsLeftLabel->setText(QString::number(player->characterPoints));
}
void MainWindow::on_VetSpinBox_valueChanged(int i)
{
	if (isPlayerOutOfPoints)
		return;
	bool playerStillHasPoints = false;
	if (playerVet > i)
	{
		playerStillHasPoints = player->ModifyAttribute(-1, 'V');
	}
	else if (playerVet < i)
	{
		playerStillHasPoints = player->ModifyAttribute(1, 'V');
	}
	if (!playerStillHasPoints)
		isPlayerOutOfPoints = true;
	else
		isPlayerOutOfPoints = false;
	playerVet = i;
	ui->pointsLeftLabel->setText(QString::number(player->characterPoints));
}

void MainWindow::on_weaponComboBox_currentIndexChanged(const QString &text)
{
	std::string nameToSearch = text.toLocal8Bit().constData();
	std::vector<Weapon> items = gm.GetWeapons();
	auto searchedItem = std::find_if(items.cbegin(), items.cend(),
		[nameToSearch](const Weapon& s) -> bool {return s.name == nameToSearch; });

	player->currentWeapon = *searchedItem;

	if (searchedItem->isTwoHanded)
	{
		ui->shieldComboBox->setEnabled(false);
		ui->shieldLabel->setText("Both hands busy!");
		ui->shieldLabel->setStyleSheet("background-color:red;");
	}
	else
	{
		ui->shieldComboBox->setEnabled(true);
		ui->shieldLabel->setText("Shield");
		ui->shieldLabel->setStyleSheet("background-color:white;");
	}
	ui->shieldLabel->adjustSize();
	ui->shieldLabel->setAlignment(Qt::AlignHCenter);
}
void MainWindow::on_armourComboBox_currentIndexChanged(const QString &text)
{
	std::string nameToSearch = text.toLocal8Bit().constData();
	std::vector<Armour> items = gm.GetArmours();
	auto searchedItem = std::find_if(items.cbegin(), items.cend(),
		[nameToSearch](const Armour& s) -> bool {return s.name == nameToSearch; });

	player->currentArmour = *searchedItem;
}
void MainWindow::on_shieldComboBox_currentIndexChanged(const QString &text)
{
	std::string nameToSearch = text.toLocal8Bit().constData();
	std::vector<Shield> items = gm.GetShields();
	auto searchedItem = std::find_if(items.cbegin(), items.cend(),
		[nameToSearch](const Shield& s) -> bool {return s.name == nameToSearch; });

	player->currentShield = *searchedItem;
}


void MainWindow::on_bSgoBackButton_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_bSteamButton_clicked()
{
	team1Size = 1;
	team2Size = 0;
	ui->resetButton->setEnabled(false);

	teamSize = 2;
	ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_bSteamButton_2_clicked()
{
	team1Size = 1;
	team2Size = 0;
	ui->resetButton->setEnabled(false);

	teamSize = 1;
	ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_goBackButton_2_clicked()
{
	team1Chars.clear();
	team1Size = 1;
	team2Chars.clear();
	team2Size = 0;
	currentTeam = 1;

	ui->stackedWidget->setCurrentIndex(2);
}
void MainWindow::on_doneButton_2_clicked()
{
	if (team1Size != teamSize
		&& team2Size != teamSize)
	{
		QMessageBox::warning(this, "Battle Start", "Teams are not full!");
		return;
	}

	team1Chars.push_back(*player);
	gm.PrepareTeams(team1Chars, team2Chars);

	std::vector<Character> charactersInPlay = gm.GetCharactersInPlay();

	int i = 0;
	ui->charactersPresentText->appendPlainText("Team 1:\n");
	while (charactersInPlay[i].GetTeam() == 1)
	{
		ui->charactersPresentText->appendPlainText(
			QString::fromStdString(charactersInPlay[i].name) + "\n");
		++i;
	}
	ui->charactersPresentText->appendPlainText("\n\nTeam 2:\n");
	while (charactersInPlay[i].GetTeam() == 2)
	{
		ui->charactersPresentText->appendPlainText(
			QString::fromStdString(charactersInPlay[i].name) + "\n");
		if (++i == charactersInPlay.size())
			break;
	}

	int j = 0;
	for (auto& it : charactersInPlay)
	{
		if (it.GetTeam() == 2)
		{
			baddiesID[j] = it.ID;
			j++;
			ui->comboBox_2->addItem(it.name.c_str());
		}
	}

	ui->attackTargetButton->setEnabled(false);
	ui->surrenderButton->setEnabled(false);
	ui->skipTurnButton->setEnabled(false);
	ui->nextTurnButton->setEnabled(false);


	ui->battleLogText->append("BATTLE STARTED\nTurn 1");
	for (auto& it : charactersInPlay)
	{
		if (it.GetTeam() == 2)
		{
			currentCharacterSelected = it;
			break;
		}
	}

	gm.SavePlayer(*player);

	ui->stackedWidget->setCurrentIndex(4);
	playTurn(0);
}
void MainWindow::on_resetButton_clicked()
{
	switch (currentTeam)
	{
	case 1:
		team1Chars.clear();
		team1Size = 1;
		break;
	case 2:
		team2Chars.clear();
		team2Size = 0;
		break;
	default:
		break;
	}

	QMessageBox::information(this,
		"Reset Team " + QString::number(currentTeam),
		"Team " + QString::number(currentTeam) + " reset!");

	updatedInfoLabel();
}
void MainWindow::on_selectButton_clicked()
{
	switch (currentTeam)
	{
	case 1:
		if (team1Size == teamSize)
		{
			QMessageBox::warning(this, "Select Character", "Team is full!");
		}
		else
		{
			team1Chars.push_back(Character(currentCharacterSelected, true));
			QMessageBox::information(this, "Select Character", "Character added!");
			++team1Size;
		}
		break;
	case 2:
		if (team2Size == teamSize)
		{
			QMessageBox::warning(this, "Select Character", "Team is full!");
		}
		else
		{
			team2Chars.push_back(Character(currentCharacterSelected, true));
			QMessageBox::information(this, "Select Character", "Character added!");
			++team2Size;
		}
		break;
	default:
		break;
	}
	if (!ui->resetButton->isEnabled())
	{
		ui->resetButton->setEnabled(true);
	}
	updatedInfoLabel();
}

void MainWindow::on_team1RadioButton_toggled(bool checked)
{
	if (checked)
	{
		currentTeam = 1;
	}
	else
	{
		currentTeam = 2;
	}
	updatedInfoLabel();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
	if (index == -1)
		return;

	currentCharacterSelected = allCharacters[index];

	QString message = "";
	std::vector<std::string> messageToTake = currentCharacterSelected.PrintCharacter();
	for (auto &it : messageToTake)
	{
		message.append(QString::fromStdString(it) + "\n");
	}

	ui->textEdit->setText(message);
}

void MainWindow::updatedInfoLabel()
{
	switch (currentTeam)
	{
	case 1:
		ui->errorLabel->setText("Team current size: " +
			QString::number(team1Size));
		break;
	case 2:
		ui->errorLabel->setText("Team current size: " +
			QString::number(team2Size));
		break;
	default:
		break;
	}
}




void MainWindow::playTurn(int characterIndex)
{
	std::vector<Character> characters = gm.GetCharactersInPlay();

	checkForDead();
	if (isGameFinished)
	{
		return;
	}

	if (currentCharacterTurn == characters.size())
	{
		++currentTurn;
		currentCharacterTurn = 0;
		characterIndex = 0;
		gm.NextTurn();

		ui->battleLogText->append("Turn " + QString::number(currentTurn + 1));
	}
	Character currentCharacter = characters[characterIndex];

	if (!isPlayerAlive)
	{
		isGameFinished = true;
		ui->battleLogText->append("You died. Game over.\n\nPress 'Next Turn' to finish.");
		ui->nextTurnButton->setEnabled(true);
		return;
	}

	if (currentCharacter.isKnockedDown)
	{
		ui->battleLogText->append(QString::fromStdString(currentCharacter.name) +
			" is knocked down for: " + QString::number(currentCharacter.knockDownTimer) + " turns.");

		ui->nextTurnButton->setEnabled(true);

		return;
	}
	if (currentCharacter.isDead)
	{
		ui->nextTurnButton->setEnabled(true);
		return;
	}

	QString messageToLog;

	if (currentCharacter.isPlayer)
	{
		ui->battleLogText->append("Your turn\n");
		ui->skipTurnButton->setEnabled(true);
		ui->attackTargetButton->setEnabled(true);
		ui->surrenderButton->setEnabled(true);
		ui->nextTurnButton->setEnabled(false);
	}
	else
	{
		messageToLog = QString::fromStdString(currentCharacter.NPCAssessSituation(characters));
		gm.UpdateCharacter(currentCharacter);
		gm.UpdateCharacter(characters[currentCharacter.currentTargetIndex]);
		gm.UpdatePlayer(player);
		if (player->isDead)
			isPlayerAlive = false;

		ui->battleLogText->append(messageToLog + "\n");

		ui->skipTurnButton->setEnabled(false);
		ui->attackTargetButton->setEnabled(false);
		ui->surrenderButton->setEnabled(false);
		ui->nextTurnButton->setEnabled(true);
	}

}

void MainWindow::on_nextTurnButton_clicked()
{
	if (isGameFinished)
	{
		ReInit();
		ui->stackedWidget->setCurrentIndex(0);
		return;
	}

	++currentCharacterTurn;

	ui->nextTurnButton->setEnabled(false);

	playTurn(currentCharacterTurn);
}
void MainWindow::on_skipTurnButton_clicked()
{
	++currentCharacterTurn;

	ui->battleLogText->append("You've decided to skip the turn.\n");

	ui->skipTurnButton->setEnabled(false);
	ui->surrenderButton->setEnabled(false);
	ui->attackTargetButton->setEnabled(false);

	playTurn(currentCharacterTurn);
}
void MainWindow::on_surrenderButton_clicked()
{
	ui->battleLogText->append("You've surrendered. Game over. \nPress 'Next Turn' to proceed");
	ui->skipTurnButton->setEnabled(false);
	ui->surrenderButton->setEnabled(false);
	ui->attackTargetButton->setEnabled(false);

	isGameFinished = true;

	ui->nextTurnButton->setEnabled(true);
}
void MainWindow::on_attackTargetButton_clicked()
{
	if (currentCharacterSelected.isDead)
	{
		ui->battleLogText->append("Target's dead.");

		ui->skipTurnButton->setEnabled(true);
		ui->surrenderButton->setEnabled(true);
		ui->attackTargetButton->setEnabled(true);

		ui->nextTurnButton->setEnabled(false);
	}
	else
	{
		QString messageToLog;
		messageToLog = QString::fromStdString(player->Attack(currentCharacterSelected));

		ui->battleLogText->append(messageToLog + "\n");

		gm.UpdateCharacter(currentCharacterSelected);
		gm.UpdatePlayer(player);

		ui->skipTurnButton->setEnabled(false);
		ui->surrenderButton->setEnabled(false);
		ui->attackTargetButton->setEnabled(false);

		ui->nextTurnButton->setEnabled(true);
	}

}
void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
	if (index == -1)
		return;

	int IDToFind = baddiesID[index];
	std::vector<Character> allcharacters = gm.GetCharactersInPlay();


	auto searchedCharacter = std::find_if(allcharacters.cbegin(), allcharacters.cend(),
		[IDToFind](const Character& c) -> bool {return c.ID == IDToFind; });

	if (searchedCharacter == allcharacters.cend())
		return;

	currentCharacterSelected = *searchedCharacter;
}

void MainWindow::checkForDead()
{
	std::vector<Character> characters = gm.GetCharactersInPlay();

	for (auto& it : characters)
	{
		if (it.GetTeam() == 2 && it.isDead)
		{
			int whatID = it.ID;

			if (whatID == baddiesID[0])
			{
				baddiesDead[0] = true;
			}
			else
			{
				baddiesDead[1] = true;
			}
		}
	}

	switch (teamSize)
	{
	case 1:
		if (baddiesDead[0])
		{
			isGameFinished = true;
			ui->battleLogText->append("All your enemies died. Press 'Next Turn' to finish.\n");
			ui->nextTurnButton->setEnabled(true);
			return;
		}
		break;
	case 2:
		if (baddiesDead[0] && baddiesDead[1])
		{
			isGameFinished = true;
			ui->battleLogText->append("All your enemies died. Press 'Next Turn' to finish.\n");
			ui->nextTurnButton->setEnabled(true);
			return;
		}
		break;
	default:
		break;
	}
}

void MainWindow::ReInit()
{
	team1Chars.clear();
	team2Chars.clear();
	team1Size = 1;
	team2Size = 0;

	*player = gm.LoadPlayer();
	teamSize = 0;
	currentTeam = 1;
	currentTurn = 0;
	currentCharacterTurn = 0;
	isGameFinished = false;
	allEnemiesDead = false;
	isPlayerAlive = true;
	isPlayerAlive = true;

	baddiesID[0] = -1;
	baddiesID[1] = -1;
	baddiesDead[0] = false;
	baddiesDead[1] = false;
	currentCharacterSelected = allCharacters[0];
	if (!isPlayerInit)
	{
		ui->prepareBattleButton->setEnabled(false);
		ui->prepareBattleButton->setText("Your character isn't ready!");
		ui->prepareBattleButton->adjustSize();
	}
	else
	{
		ui->prepareBattleButton->setEnabled(true);
		ui->prepareBattleButton->setText("Prepare the battle");
		ui->prepareBattleButton->adjustSize();
	}
	if (currentTeam == 1)
	{
		currentTeam = 1;
		ui->errorLabel->setText("Team current size: " +
			QString::number(team1Chars.size() + 1));
	}
	else
	{
		currentTeam = 2;
		ui->errorLabel->setText("Team current size: " +
			QString::number(team2Chars.size()));
	}
}
