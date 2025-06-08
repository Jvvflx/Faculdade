#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <avr/wdt.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int led1 = 8;
const int led2 = 9;
const int led3 = 10;
const int button_entrada = 7;

int passageiros[2] = {0, 0};
int contadorEstacao = 0;
int contadorDia = 0;
bool lastButton_entradaState = LOW;
bool trem_parado = HIGH;
unsigned long tempoInicio = 0;
unsigned long tempoDiaInicio = 0;
bool fimDoDia = LOW;
const unsigned long tempoEmbarque = 15000; // 15 segundos para embarque
const unsigned long tempoDia = 40000;      // 40 segundos para um "dia"

// Exibe os passageiros no monitor seiral
void exibirPassageiros() {
    Serial.println("Passageiros:");

    Serial.println("Ontem: " + String(passageiros[0]));
    Serial.println("Antes de Ontem: " + String(passageiros[1]));
    int resultado = abs(passageiros[0] - passageiros[1]);
    Serial.println("Diferenca dos dias anteriores: " + String(resultado));


    delay(2000);
}

// Salva os valores na EEPROM
void salvarEEPROM() {
    for (int i = 0; i < 2; i++) {
        EEPROM.put(i * sizeof(int), passageiros[i]);
    }

}

// Carrega os valores armazenados na EEPROM
void carregarEEPROM() {
    Serial.println("\nCarregando dias anteriores da EEPROM...");
    for (int i = 0; i < 2; i++) {
        EEPROM.get(i * sizeof(int), passageiros[i]);

    }
}

// Atualiza os valores dos últimos dias
void atualizarPassageiros(int qtd) {
    passageiros[1] = passageiros[0];
    passageiros[0] = qtd;
}

// Representação de chegada e saída do metrô
void piscar() {
    for (int i = 0; i < 10; i++) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, LOW);
        delay(200);
        digitalWrite(led2, HIGH);
        digitalWrite(led1, LOW);
        delay(200);
    }
    digitalWrite(led2, LOW);
    digitalWrite(led1, LOW);
}

// Comando de contagem dos passageiros
void press() {
    contadorEstacao++;
    digitalWrite(led3, HIGH);
    delay(300);
    digitalWrite(led3, LOW);
}

// Comando de reset do sistema ao fim do dia
void resetarSistema() {
    Serial.println("Resetando sistema...");
    lcd.clear();
    lcd.print("Resetando...");
    delay(2000);

    asm volatile ("jmp 0");  // Pula para o endereço de reset do bootloader
}


void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd.print("Metro chegou!");
    trem_parado = HIGH;

    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(button_entrada, INPUT);

    piscar();
    delay(500);

    lcd.clear();
    lcd.print("Embarque!");

    carregarEEPROM();
    exibirPassageiros();
    tempoInicio = millis();     // Marca o início do embarque
    tempoDiaInicio = millis();  // Marca o início do "dia"
}

void loop() {
    bool button_entradaState = digitalRead(button_entrada);

    // Verifica se um passageiro entrou dentro do metrô
    if (button_entradaState == HIGH && lastButton_entradaState == LOW && trem_parado == HIGH) {
        press();
    }

    lastButton_entradaState = button_entradaState;

    // Verifica se o tempo de embarque passou
    if (millis() - tempoInicio >= tempoEmbarque) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Portas fechando!");
        piscar();

        trem_parado = LOW;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Passageiros:");
        lcd.setCursor(0, 1);
        lcd.print(String(contadorEstacao));

        Serial.println("Metro esta partindo da estacao...");
        delay(2000);
        Serial.println("Metro esta chegando na estacao...");
        delay(1000);

        contadorDia += contadorEstacao;
        contadorEstacao = 0;

        trem_parado = HIGH;
        lcd.clear();
        lcd.print("Metro chegou!");
        delay(500);
        lcd.clear();
        lcd.print("Embarque!");

        tempoInicio = millis(); // Reinicia o tempo de embarque
    }

    // Verifica se já passou um "dia" (40 segundos neste caso)
    if (millis() - tempoDiaInicio >= tempoDia && fimDoDia == LOW) {
        fimDoDia = HIGH;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Encerrando dia.");
        delay(1000);
        atualizarPassageiros(contadorDia);
        salvarEEPROM();
        resetarSistema(); // Reinicia o sistema após o dia acabar
    }
}
