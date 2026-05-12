#include <reg51.h>

sbit RS = P3^5;
sbit EN = P3^6;
#define LCD_DATA P2

sbit ButtonA = P1^0;
sbit ButtonB = P1^1;
sbit ButtonC = P1^2;
sbit ResultBtn = P1^3;
sbit ResetBtn = P1^4;

void wait(unsigned int time) {
    unsigned int i, j;
    for(i=0; i<time; i++)
        for(j=0; j<125; j++);
}

void sendCommand(unsigned char cmd) {
    LCD_DATA = cmd;
    RS = 0;
    EN = 1;
    wait(1);
    EN = 0;
}

void sendData(unsigned char value) {
    LCD_DATA = value;
    RS = 1;
    EN = 1;
    wait(1);
    EN = 0;
}

void writeText(char *str) {
    while(*str) {
        sendData(*str++);
    }
}

void displayCount(unsigned int count) {
    sendData((count / 10) + '0'); 
    sendData((count % 10) + '0'); 
}

void main() {
    unsigned int countA = 0, countB = 0, countC = 0;

    sendCommand(0x38);
    sendCommand(0x0C);
    sendCommand(0x01);
    
    writeText(" VOTING MACHINE"); 
    wait(200);

    while(1) {
        sendCommand(0x80);
        writeText("A:"); displayCount(countA);
        writeText(" B:"); displayCount(countB);
        writeText(" C:"); displayCount(countC);

        sendCommand(0xC0);
        writeText(" Cast Your Vote! ");

        if(ButtonA == 0) { 
            countA++; 
            if(countA > 99) countA = 0; 
            while(ButtonA == 0); 
        }
        if(ButtonB == 0) { 
            countB++; 
            if(countB > 99) countB = 0; 
            while(ButtonB == 0); 
        }
        if(ButtonC == 0) { 
            countC++; 
            if(countC > 99) countC = 0; 
            while(ButtonC == 0); 
        }

        if(ResultBtn == 0) {
            sendCommand(0x01);
            writeText(" Final Result:");
            sendCommand(0xC0); 
            
            // --- UPDATED WINNER & TIE LOGIC ---
            if(countA == 0 && countB == 0 && countC == 0) {
                writeText(" No Votes Cast  ");
            }
            else if(countA > countB && countA > countC) writeText("Winner: A      ");
            else if(countB > countA && countB > countC) writeText("Winner: B      ");
            else if(countC > countA && countC > countB) writeText("Winner: C      ");
            else if(countA == countB && countA > countC) writeText("Tie: A & B     ");
            else if(countA == countC && countA > countB) writeText("Tie: A & C     ");
            else if(countB == countC && countB > countA) writeText("Tie: B & C     ");
            else writeText("Tie: All Three ");
            
            wait(2000); 
            while(ResultBtn == 0);
            sendCommand(0x01);
        }

        if(ResetBtn == 0) {
            countA = 0; countB = 0; countC = 0;
            sendCommand(0x01);
            writeText(" System Reset...");
            wait(500);
            while(ResetBtn == 0);
            sendCommand(0x01);
        }
    }
}