
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

const byte numRows = 4;
const byte numCols = 4;

char keymap[numRows][numCols] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[numRows] = {9, 8, 7, 6};
byte colPins[numCols] = {5, 4, 3, 2};

Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
LiquidCrystal_I2C lcd_1(0x27, 16, 2);

int recebeuNome = 0;
int contador = 0;
int comeco = 1;
char resp[16];
char recebido[16];
int nivelAtual = 1;
String NomeAtual = "";
String Nome;
String nomes[3] = {"","",""};
int ranking[3] = {0, 0, 0};
int limite = 0;

const char caracteresDisponiveis[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D'};


void exibirRanking() {
    lcd_1.clear();
    lcd_1.setCursor(0, 0);
    lcd_1.print("Ranking:");
  	Serial.print("Ranking:\n");
    lcd_1.setCursor(0, 1);
    lcd_1.print("1:" + String(ranking[0]) + " 2:" + String(ranking[1]) + " 3:" + String(ranking[2]));
  	Nome = ("1:" + nomes[0] + " " + String(ranking[0]));
  	Serial.println(Nome);
    Nome = ("2:" + nomes[1] + " " + String(ranking[1]));
  	Serial.println(Nome);
    Nome = ("3:" + nomes[2] + " " + String(ranking[2]));
  	Serial.println(Nome);
    delay(2000);
	lcd_1.clear();
}

bool verificaSequencia()
{
    for (int i = 0; i < limite; i++)
    {
        if (resp[i] != recebido[i]) 
        {
            return false;
        }
    }
    return true;
}

void salvarRankingEEPROM() {
    for (int i = 0; i < 3; i++) {
        EEPROM.put(i * 20, ranking[i]);
        for (int j = 0; j < 10; j++) {
            EEPROM.write(i * 20 + 4 + j, j < nomes[i].length() ? nomes[i][j] : '\0');
        }
    }
}

void carregarRankingEEPROM() {
    Serial.println("\nCarregando ranking da EEPROM...");
    for (int i = 0; i < 3; i++) {
        EEPROM.get(i * 20, ranking[i]);
        char buffer[11];
        for (int j = 0; j < 10; j++) {
            buffer[j] = EEPROM.read(i * 20 + 4 + j);
        }
        buffer[10] = '\0';
        nomes[i] = String(buffer);

        // Exibe os valores carregados no monitor serial
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(nomes[i]);
        Serial.print(" - Nivel: ");
        Serial.println(ranking[i]);
    }
}


void atualizarRanking(int nivel, String Nome) {
    for (int i = 0; i < 3; i++) {
        if (nivel >= ranking[i]) {
            for (int j = 2; j > i; j--) {
                ranking[j] = ranking[j - 1];
                nomes[j] = nomes[j - 1];
            }
            ranking[i] = nivel;
            nomes[i] = Nome;
            salvarRankingEEPROM();
            break;
        }
    }
}

void setup() {
    Serial.begin(9600);
    lcd_1.init();
    lcd_1.backlight();
    lcd_1.print("Jogo Iniciado!");
    delay(500);
    randomSeed(analogRead(0));
    carregarRankingEEPROM();
}

void loop()
{
    if (recebeuNome == 0){
        lcd_1.setCursor(0, 0);
        lcd_1.print("Digite seu nome");
        
        if (Serial.available() > 0){
            char buffer[7]; // Buffer para armazenar o nome (6 caracteres + '\0')
            int tamanho = Serial.readBytesUntil('\n', buffer, 6);
            buffer[tamanho] = '\0'; // Garante que a string termine corretamente
            
            NomeAtual = String(buffer);
            NomeAtual.trim(); // Remove espaços extras

            if(NomeAtual == "EEPROM"){
                carregarRankingEEPROM();
            }
            
            else if (NomeAtual.length() > 0) { // Garante que o nome não esteja vazio
                recebeuNome = 1;
                lcd_1.clear();
                lcd_1.setCursor(0, 0);
                lcd_1.print("Bem vindo(a)!");
                lcd_1.setCursor(0, 1);
                lcd_1.print(NomeAtual);
                delay(1000);
            }
        }
    } else{
    if (comeco == 1)
    {
        lcd_1.clear();
        lcd_1.setCursor(0,0);
        lcd_1.print("Memorize-Nivel" + String(nivelAtual));
        delay(700);

        limite = min(16, nivelAtual + 3); // Número de dígitos na sequência
        int numCaracteres = min(10 + (nivelAtual / 3), 14); // Ajuste da dificuldade
        int aumentarVelocidade = max(200, (numCaracteres * limite * nivelAtual)); // Velocidade em que o texto aparece em cada nível

        for (int i = 0; i < limite; i++)
        {
            resp[i] = caracteresDisponiveis[random(0, numCaracteres)];
            lcd_1.setCursor(i, 1);
            lcd_1.print((char)resp[i]);
          	delay(min(1000, 1000 - aumentarVelocidade));
          	lcd_1.setCursor(i, 1);
            lcd_1.print(' ');
            lcd_1.setCursor(i, 1);
        }

        lcd_1.clear();
        lcd_1.setCursor(0, 0);
        lcd_1.print("Repita-Nivel" + String(nivelAtual));

        comeco = 0;
        contador = 0;
    }

    char keypressed = myKeypad.getKey();

    if (keypressed == '*') // Verifica a sequência quando '*' é pressionado
    {
        if (verificaSequencia() && contador > 0)
        {
            nivelAtual++;
            lcd_1.clear();
            lcd_1.setCursor(0, 0);
            lcd_1.print("Voce ganhou!");
            lcd_1.setCursor(0, 1);
            lcd_1.print("Nivel: " + String(nivelAtual));
            delay(100);
        }
        else
        {
            nivelAtual = max(1, nivelAtual - 1); // Garante que o nível mínimo seja 1
            lcd_1.clear();
            lcd_1.setCursor(0, 0);
            lcd_1.print("Voce perdeu!");
            lcd_1.setCursor(0, 1);
            lcd_1.print("Nivel: " + String(nivelAtual));
            delay(100);
        }

        delay(1000);
        comeco = 1;
    }

    else if (keypressed != NO_KEY && contador < 16) // Armazena a tecla apenas se não for '*'
    {
        recebido[contador] = keypressed;
        lcd_1.setCursor(contador, 1);
        lcd_1.print(keypressed);
        Serial.println(keypressed);
        contador++;
    }

    if (keypressed == '#'){
        lcd_1.clear();
        lcd_1.setCursor(0, 0);
        lcd_1.print("Encerrando jogo");
        delay(1000);
        atualizarRanking(nivelAtual,NomeAtual);
        nivelAtual = 1;
        exibirRanking();
        comeco = 1;
      	recebeuNome = 0;
    }
  }
}
