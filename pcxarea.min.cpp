#include <iostream>
#include <fstream>
#include <vector>

struct PCX_Header;
int inline imgWidth(PCX_Header &header);
int inline imgHeight(PCX_Header &header);
uint8_t getByte(PCX_Header &header, std::ifstream &inFile);
int isPowerOfTwo(unsigned int x);

struct PCX_Header
{
    uint8_t manufacturer;
    uint8_t version;
    uint8_t encoding;
    uint8_t bitsPerPlane;
    uint16_t Xmin;
    uint16_t Ymin;
    uint16_t Xmax;
    uint16_t Ymax;
    uint16_t VertDPI;
    uint16_t HorzDPI;
    uint8_t palette[48];
    uint8_t reserved;
    uint8_t colorPlanes;
    uint16_t bytesPerPlaneLine;
    uint16_t paltype;
    uint16_t hScrSize;
    uint16_t vScrSize;
};

class CVrchol {
public:
    CVrchol(int aZacatek, int aDelka, int aBarva);
    int zacatek();
    int konec();
    int delka();
    int barva();
    int zacatekX(PCX_Header &header);
    int konecX(PCX_Header &header);
    bool jeSoused(CVrchol &v, PCX_Header &header);
    void pridejHranu(CVrchol &v);
    bool bylProhledan();
    void setBylProhledan(bool);
    int zjistiHloubku();
private:
    int pPozice;
    int pDelka;
    int pBarva;
    bool pBylProhledan;
    std::vector<CVrchol *> e;
};

CVrchol::CVrchol(int aZacatek, int aDelka, int aBarva) {
    pPozice = aZacatek;
    pDelka = aDelka;
    pBarva = aBarva;
    pBylProhledan = false;
    e =  *new std::vector<CVrchol *>();
}

int CVrchol::zacatek() {return pPozice;}
int CVrchol::konec() {return pPozice + pDelka;}
int CVrchol::delka() {return pDelka;}
int CVrchol::barva() {return pBarva;}
int CVrchol::zacatekX(PCX_Header &header) {return pPozice % (header.Xmax - header.Xmin + 1);}
int CVrchol::konecX(PCX_Header &header) {return (pPozice + pDelka - 1) % (header.Xmax - header.Xmin + 1);}
void CVrchol::pridejHranu(CVrchol &v) {e.push_back(&v);}
bool CVrchol::bylProhledan() {return pBylProhledan;}
void CVrchol::setBylProhledan(bool aBylProhledan) {pBylProhledan = aBylProhledan;}

bool CVrchol::jeSoused(CVrchol &v, PCX_Header &header) {
    int width = header.Xmax - header.Xmin + 1;
    if (v.zacatek() / width == zacatek() / width) return false; // pro pripad, ze jsou na stejnem radku
    return std::min(v.konecX(header), konecX(header)) - std::max(v.zacatekX(header), zacatekX(header)) >= 0;
}

int CVrchol::zjistiHloubku() {
    if (bylProhledan()) return 0;
    else setBylProhledan(true);
    
    int tmp = delka();
    
    std::vector<CVrchol *>::iterator it;
    for (it = e.begin(); it < e.end(); it++) {
        tmp += (*it)->zjistiHloubku();
    }
    
    return tmp;
}

int inline imgWidth(PCX_Header &header) {return header.Xmax - header.Xmin + 1;}
int inline imgHeight(PCX_Header &header) {return header.Ymax - header.Ymin + 1;}

uint8_t getByte(PCX_Header &header, std::ifstream &inFile) {
    uint8_t byte;
    inFile.read((char *) &byte, sizeof(byte));
    return byte;
}

int isPowerOfTwo (unsigned int x) {
    return ((x != 0) && ((x & (~x + 1)) == x));
}

int main(int argc, const char * argv[]) {
    if (argc != 2) return 1;
    std::ifstream inFile;
    inFile.open(argv[1], std::ios::binary);
    
    if(! inFile.is_open()) return 1;
    
    struct PCX_Header header;
    inFile.read((char *) &header, sizeof(struct PCX_Header));
    
    if (header.manufacturer != 0x0A) return 1;
    if (header.encoding != 1) return 1;
    if (8 != header.bitsPerPlane) return 1;
    if ((header.Xmax - header.Xmin) < 1)  return 1;
    if (1 != header.colorPlanes) return 1;
    if (2 != header.paltype) return 1;
    if (!(isPowerOfTwo(imgWidth(header)) || imgWidth(header) < 8)) return 1;
    
    unsigned int numPixels = imgWidth(header) * imgHeight(header);
    unsigned int readPixels = 0;
    
    inFile.seekg(0x80, std::ios_base::beg);
    uint8_t byte = 0; 
    uint8_t runCount = 0; 
    uint8_t runValue = 0;
    
    std::vector<CVrchol *> vrcholy;
    int ukPredchozi = 0;
    CVrchol *v;
    
    do {
        byte = getByte(header, inFile);
        if ((byte & 0xC0) == 0xC0)  {
            runCount = byte & 0x3F;
            inFile.read((char *) &runValue, sizeof(runValue));
        } else {
            runCount = 1;
            runValue = byte;
        }
        
        v = new CVrchol(readPixels, runCount, runValue);
        vrcholy.push_back(v);
        
        if (vrcholy.size() > 1) {
            CVrchol *predposledni = vrcholy.at(vrcholy.size() - 2);
            CVrchol *posledni = vrcholy.at(vrcholy.size() - 1);
            if (predposledni->barva() == posledni->barva()) {
                predposledni->pridejHranu(*posledni);
                posledni->pridejHranu(*predposledni);
            }
        }
        readPixels += runCount;
    } while (v->konecX(header) != imgWidth(header)-1);
    
    do {
        byte = getByte(header, inFile);
                
        if ((byte & 0xC0) == 0xC0)  {
            runCount = byte & 0x3F;
            inFile.read((char *) &runValue, sizeof(runValue));
        } else {
            runCount = 1;
            runValue = byte;
        }
        
        CVrchol *v = new CVrchol(readPixels, runCount, runValue);
        vrcholy.push_back(v);
        
        while (v->jeSoused(*vrcholy.at(ukPredchozi), header)) {
            if (v->barva() == vrcholy.at(ukPredchozi)->barva()) {
                v->pridejHranu(*vrcholy.at(ukPredchozi));
                vrcholy.at(ukPredchozi)->pridejHranu(*v);
            }
            
            if (vrcholy.at(ukPredchozi)->konecX(header) - v->konecX(header) > 0) break;
            else ukPredchozi++;
        }
        
        CVrchol *predposledni = vrcholy.at(vrcholy.size() - 2);
        CVrchol *posledni = vrcholy.at(vrcholy.size() - 1);
        
        if ((predposledni->zacatek() / imgWidth(header) == posledni->zacatek() / imgWidth(header)) &&
            (predposledni->barva() == posledni->barva())) {
            predposledni->pridejHranu(*posledni);
            posledni->pridejHranu(*predposledni);
        }
        
        readPixels += runCount;
        
    } while (readPixels < numPixels);
    
    int max = 0;
    for (std::vector<CVrchol *>::iterator it = vrcholy.begin(); it < vrcholy.end(); it++)
        max = std::max(max, (*it)->zjistiHloubku());
    std::cout << max << std::endl;

    return 0;
}
