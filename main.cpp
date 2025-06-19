#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include <string>
#include <random>
#include <set>
#include <vector>
#include <deque>
#include <optional>



using namespace std;
using namespace sf;

enum class AlienMovementType {
    STATIC,
    CIRCULAR
};

class Alien {
private:
    Sprite sprite;
    const Texture* texture;
    bool isShooting = false;
    int mviata_alien;
    AlienMovementType movementType = AlienMovementType::STATIC;
    float mangle;

    // Pentru miscare circulara
    Vector2f center;
    float angle = 0.f;
    float mradius = 50.f;
    float speed = 1.f;

public:
    Alien(const Vector2f& position, const Texture& tex, int viata = 1, AlienMovementType type = AlienMovementType::STATIC) : texture(&tex), mviata_alien(viata), movementType(type), center(position) {
        sprite.setTexture(*texture);
        sprite.setPosition(position);
    }

    void update(float deltaTime) {
        //actualizeaza miscarea extraterestriilor in functie de  tipul lor de miscare
        if (movementType == AlienMovementType::STATIC)
            return;
        else if (movementType == AlienMovementType::CIRCULAR) {
            angle += speed * deltaTime;
            float totalAngle = angle + mangle;
            sprite.setPosition(center.x + mradius * cos(totalAngle),center.y + mradius * sin(totalAngle));
        }
    }

    //seteaza tipul de miscare al extraterestrului
    void setMovementType(AlienMovementType type) {
        movementType = type;
    }

    //seteaza centrul cercului pe care il forrmeaza extraterestrii
    void setCenter(Vector2f c) {
        center = c;
    }

    //afiseaza extraterestrul
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }

    //ia ungihiul de la care pleaca extraterestrul
    float getAngle() {
        return angle;
    }

    //seteaza unghiul de la care pleaca
    void setAngle(float angle) {
        mangle = angle;
    }

    //seteaza raza cercului
    void setRadius(float radius) {
        mradius = radius;
    }

    //verifica daca poate trage extraterestrul
    bool tryToShoot(int nivel) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        float chance = dis(gen);

        float shootingChance = std::min(0.05f * nivel, 0.3f);
        return chance < shootingChance;
    }

    //returneaza granitele maxime ale sprite-ului
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
    //setez viata extraterestrului
    void setviataAlien(int viata_alien) {
        mviata_alien = viata_alien;
    }
    //aflui viata extraterestrului
    int getviataAlien() {
        return mviata_alien;
    }

    //returneaza pozitia extraterestrului
    Vector2f getPosition() const {
        return sprite.getPosition();
    }
};

class RomanianFinalBoss {
private:
    Sprite sprite;
    const Texture* texture;
    int mviata_boss = 1000;
    deque<Vector2f> rachetaPozitii; // Salveaza poziția rachetei în timp
    Clock fireClock;                // Cronometru pentru tragere
    float timeAccumulator = 0.f;
    int numarGloanteTrase = 0;
    bool inPauza = false;
    Clock pauzaClock;

public:
    RomanianFinalBoss(const Vector2f& position, const Texture& tex) : texture(&tex){
        sprite.setTexture(*texture);
        sprite.setPosition(position);
    }

    //misca boss-ul stanga dreapta
    void update(float deltaTime, RenderWindow& window, Vector2f rachetaPoz) {
        timeAccumulator += deltaTime;

        rachetaPozitii.push_back(rachetaPoz);
        if (rachetaPozitii.size() > 30)
            rachetaPozitii.pop_front();

        static float speed = 100.f;
        Vector2f pos = sprite.getPosition();
        pos.x += speed * deltaTime;

        // Schimba directia la marginea ferestrei
        if (pos.x < 0 || pos.x + sprite.getGlobalBounds().width > window.getSize().x) {
            speed = -speed;
        }

        sprite.setPosition(pos);
    }

    //modifica viata boss-ului
    void takeDamage(int damage) {
        mviata_boss -= damage;
        if (mviata_boss < 0)
            mviata_boss = 0;
    }

    //returneaza viata
    int getviataBoss() {
        return mviata_boss;
    }

    // Genereaza un mai multe gloante spre pozitia de acum 0.5 secunde a rachetei cu o pauza de 0 secunda intre doua valuri de multe gloante
    optional<Vector2f> fire() {
        if (inPauza) {
            if (pauzaClock.getElapsedTime().asSeconds() >= 1.f) {
                inPauza = false;
                numarGloanteTrase = 0;
            } else {
                return nullopt;
            }
        }

        if (fireClock.getElapsedTime().asSeconds() >= 0.2f && !rachetaPozitii.empty()) {
            fireClock.restart();
            numarGloanteTrase++;

            if (numarGloanteTrase >= 10) {
                inPauza = true;
                pauzaClock.restart();
            }

            Vector2f target = rachetaPozitii.front();
            Vector2f dir = target - sprite.getPosition();
            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len != 0) dir /= len;

            return dir;
        }

        return nullopt;
    }

    //deseneaza Boss-ul
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }

    //returneaza marginiile boss-ului
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    Vector2f getPosition() const {
        return sprite.getPosition();
    }
};

class Bullet_Racheta {
private:
    Sprite sprite;
    const Texture* texture;
    float speed = 300.f;
    Vector2f direction;
public:
    Bullet_Racheta(const Vector2f& startPos, const Texture& tex, Vector2f dir = Vector2f(0.f, -1.f))  : texture(&tex), direction(dir) {
        sprite.setTexture(*texture);
        sprite.setPosition(startPos);
    }
    //misca glontul in sus
    void update(float deltaTime) {
        sprite.move(direction * speed * deltaTime);
    }
    //verifica daca a iesit din ecran glontul
    bool isOffScreen(float screenHeight) const {
        return sprite.getPosition().y > screenHeight;
    }
    //afiseaza glontul rachetei
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    //returneaza marginile glontului
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

};

class Bullet {
private:
    Sprite sprite;
    const Texture* texture;
    float speed = 200.f;
public:
    Bullet(const Vector2f& startPos, const Texture& tex) : texture(&tex) {
        sprite.setTexture(*texture);
        sprite.setPosition(startPos);
    }
    //misca glontul in jos deoarece e a extraterestrului
    void update(float deltaTime) {
        sprite.move(0.f, speed * deltaTime);  // Misca glontul în jos
    }
    //verifica daca a iesit din ecran glontul
    bool isOffScreen(float screenHeight) const {
        return sprite.getPosition().y > screenHeight;
    }
    //afiseaza glontul
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    //returneaza marginile glontului
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

};

class BulletBoss {
private:
    Sprite sprite;
    const Texture* texture;
    Vector2f direction;
    float speed = 300.f;
public:
    BulletBoss(const Vector2f& startPos, const Texture& tex, Vector2f dir = {0.f, 1.f}) : texture(&tex), direction(dir) {
        sprite.setTexture(*texture);
        sprite.setPosition(startPos);
    }
    //misca glontul in directia data de deltaTime
    void update(float deltaTime) {
        sprite.move(direction * speed * deltaTime);
    }
    //verifica daca a iesit glontul din ecran
    bool isOffScreen(float screenHeight) const {
        return sprite.getPosition().y > screenHeight || sprite.getPosition().y < 0;
    }
    //afiseaza glontul
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    //returneaza marginile glontului
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
};

class Upgrade1 {
    private:
    Sprite sprite;
    const Texture* texture;
    float speed = 50.f;
    public:
    Upgrade1(const Vector2f& startPos, const Texture& tex) : texture(&tex) {
        sprite.setTexture(*texture);
        sprite.setPosition(startPos);
    }
    //deplaseaza upgrade-ul in jos
    void update(float deltaTime) {
        sprite.move(0.f, speed * deltaTime);  // Misca glontul în jos
    }
    //verifica daca a iesit din ecran
    bool isOffScreen(float screenHeight) const {
        return sprite.getPosition().y > screenHeight;
    }
    //afiseaza upgrade-ul
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    //returneaza marginile obiectului
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
    //returneaza pozitia sprite-ului
    Vector2f getPosition() const {
        return sprite.getPosition();
    }

};

class ViataBonus {
private:
    Sprite sprite;
    const Texture* texture;
    float speed = 100.f;
public:
    ViataBonus(const Vector2f& startPos, const Texture& tex) : texture(&tex) {
        sprite.setTexture(*texture);
        sprite.setPosition(startPos);
    }
    //deplaseaza viata in jos
    void update(float deltaTime) {
        sprite.move(0.f, speed * deltaTime);  // Misca glontul în jos
    }
    //verifica daca a iesit din ecran
    bool isOffScreen(float screenHeight) const {
        return sprite.getPosition().y > screenHeight;
    }
    //afiseaza viata
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    //returneaza marginile obiectului
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
    //returneaza pozitia sprite-ului
    Vector2f getPosition() const {
        return sprite.getPosition();
    }

};

class Racheta {
private:
    Sprite sprite;
    const Texture& texture;
    int mVieti;
    int mUpgrade1 = 0;
    int mdamage = 1;

public:
    Racheta(const Texture& tex) : texture(tex) {
        sprite.setTexture(texture);
        sprite.setPosition(960, 900);
    }
    // actualizeaza pozitia rachetei
    void update(Vector2i mousePos) {
        sprite.setPosition(mousePos.x - sprite.getGlobalBounds().width / 2, mousePos.y - sprite.getGlobalBounds().height / 2);
    }
    //afiseaza racheta
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    //seteaza vietile rachetei
    void setVieti(int vieti) {
        mVieti = vieti;
    }
    //seteaza nivelul de upgrade al rachetei
    void setUpgrade1(int upgrade1) {
        mUpgrade1 = upgrade1;
    }
    //returneaza vietile rachetei
    int getVieti() const {
        return mVieti;
    }
    //seteaza damage-ul gloantelor
    void setDamage(int damage) {
        mdamage = damage;
    }
    //returneaza damage-ul gloantelor
    int getDamage() {
        return mdamage;
    }
    //returneaza nivelul de upgrade
    int getUpgrade1() {
        return mUpgrade1;
    }
    //returneaza coordonatele marginilor rachetei
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
    //returneaza pozitia rachetei
    Vector2f getPosition() const {
        return sprite.getPosition();
    }
};

//face toate gloantele rachetei sa mearga in sus si le sterge dupa ce ies din ecran
void urcaGlontul(vector<Bullet_Racheta>& bullets, float deltaTime, RenderWindow& window) {
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
        bullet.draw(window);
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(), [&window](const Bullet_Racheta& b) {
            return b.isOffScreen(window.getSize().y);
        }), bullets.end());
}

//face toate upgrade-urile cazute din monstrii sa mearga in jos si le sterge dupa ce ies din ecran
void cadeUpgrade1(vector<Upgrade1>& upgrade1, float deltaTime, RenderWindow& window) {
    for (auto& bullet : upgrade1) {
        bullet.update(deltaTime);
        bullet.draw(window);
    }

    upgrade1.erase(remove_if(upgrade1.begin(), upgrade1.end(), [&window](const Upgrade1& u) {
            return u.isOffScreen(window.getSize().y);
        }), upgrade1.end());
}

//face toate vietile cazute din monstrii sa mearga in jos si le sterge dupa ce ies din ecran
void cadeViata(vector<ViataBonus>& viata, float deltaTime, RenderWindow& window) {
    for (auto& bullet : viata) {
        bullet.update(deltaTime);
        bullet.draw(window);
    }

    viata.erase(remove_if(viata.begin(), viata.end(), [&window](const ViataBonus& u) {
            return u.isOffScreen(window.getSize().y);
        }), viata.end());
}

//face toate gloantele monstrilor sa mearga in jos si le sterge dupa ce ies din ecran
void cadeGlontul(vector<Bullet>& bullets, float deltaTime, RenderWindow& window) {
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
        bullet.draw(window);
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(), [&window](const Bullet& b) {
            return b.isOffScreen(window.getSize().y);
        }), bullets.end());
}

//face toate gloantele boss-ului sa mearga inspre racheta si le sterge dupa ce ies din ecran
void cadeGlontulBoss(vector<BulletBoss>& bullets, float deltaTime, RenderWindow& window, Racheta& racheta) {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
        bulletIt->update(deltaTime);
        bulletIt->draw(window);

        // Coliziune cu racheta
        if (bulletIt->getGlobalBounds().intersects(racheta.getGlobalBounds())) {
            racheta.setVieti(racheta.getVieti() - 1); // scade viața rachetei
            bulletIt = bullets.erase(bulletIt);       // elimină glonțul
        } else if (bulletIt->isOffScreen(window.getSize().y)) {
            bulletIt = bullets.erase(bulletIt);       // elimină glonțul dacă e în afara ecranului
        } else {
            ++bulletIt;
        }
    }
}

//initializeaza extraterestrii din primul nivel si din primul val al celui de al doilea nivel
void initializeAliens(vector<vector<Alien>>& aliens, const Texture& alienTexture, int nivel, AlienMovementType movement = AlienMovementType::STATIC) {
    for (int row = 0; row < 10; ++row) {
        vector<Alien> alienRow;
        for (int col = 0; col < 18; ++col) {
            Vector2f pos(100 + col * 100, 70 + row * 70);
            Alien alien(pos, alienTexture, nivel, movement);
            alienRow.push_back(alien);
        }
        aliens.push_back(alienRow);
    }
}
//initializeaza extraterestrii pentru al doilea val din al doilea nivel
void initializeAliensCircle(vector<Alien>& aliens, const Texture& alienTexture, int nivel, AlienMovementType movement = AlienMovementType::CIRCULAR) {
    int totalAliens = 24;
    float centerX = 960; // centrul ecranului (pentru 1920x1080)
    float centerY = 540;
    float radius = 400.f;

    for (int i = 0; i < totalAliens; ++i) {
        float angle = (2 * M_PI * i) / totalAliens;
        Vector2f pos(centerX + radius * cos(angle), centerY + radius * sin(angle));
        Alien alien(pos, alienTexture, nivel, movement);
        alien.setCenter(Vector2f(centerX, centerY));
        alien.setRadius(radius);
        alien.setAngle(angle);
        aliens.push_back(alien);
    }
}

//permite extraterestrilor sa traga glonturi catre jucator o data pe secunda,
//in functie de o probabilitate controlata de nivel
void updateShooting(Clock& shootClock, vector<vector<Alien>>& aliens, vector<Bullet>& bullets, const Texture& bulletTexture, double level) {
    if (shootClock.getElapsedTime().asSeconds() >= 1) {
        for (auto& row : aliens) {
            for (auto& alien : row) {
                if (alien.tryToShoot(level)) {
                    Vector2f alienPos = alien.getPosition();
                    Bullet newBullet(Vector2f(alienPos.x + 25, alienPos.y + 50), bulletTexture);
                    bullets.push_back(newBullet);
                }
            }
        }
        shootClock.restart();
    }
}
//permite extraterestrilor care se invart in cerc sa traga glonturi catre jucator o data pe secunda,
//in functie de o probabilitate controlata de nivel
void updateShootingCircleAlien(Clock& shootClock, vector<Alien>& aliens, vector<Bullet>& bullets, const Texture& bulletTexture, double level) {
    if (shootClock.getElapsedTime().asSeconds() >= 1) {
        for (auto& row : aliens) {
                if (row.tryToShoot(level)) {
                    // Creezi un glonț sau marchezi că trage
                    Vector2f alienPos = row.getPosition();
                    Bullet newBullet(Vector2f(alienPos.x + 25, alienPos.y + 50), bulletTexture);
                    bullets.push_back(newBullet);

            }
        }
        shootClock.restart();
    }
}

//Actualizeaza pozitia extraterestriilor intr-o matrice
void updateAliens(vector<vector<Alien>>& aliens, float deltaTime) {
    for (auto& row : aliens) {
        for (auto& alien : row) {
            alien.update(deltaTime);
        }
    }
}

bool Jocul(RenderWindow& window, Racheta& racheta) {
    //Se seteaza toate asseturile necesare cum ar fi texturile si fontul
    Texture texturaJoc;
    if (!texturaJoc.loadFromFile("assets/FundalNivel1.jpg"))
        return false;
    Texture texturaViataBonus;
    if(!texturaViataBonus.loadFromFile("assets/viata_bonus.png"))
        return false;
    Texture alien_texture;
    if(!alien_texture.loadFromFile("assets/extraterestru.png"))
        return false;

    Texture bullet_racheta_texture;
    if(!bullet_racheta_texture.loadFromFile("assets/glont_racheta.png"))
        return false;
    Texture alien_bullet_texture;
    if (!alien_bullet_texture.loadFromFile("assets/glont_alien.png"))
        return false;
    Texture upgrade1_texture;
    if (!upgrade1_texture.loadFromFile("assets/upgrade1.png"))
        return false;
    Font font;
    if (!font.loadFromFile("assets/FontulMeu.otf"))
        return false;
    //Se seteaza fundalul pe marimea eranului
    Sprite spriteJoc;
    spriteJoc.setTexture(texturaJoc);
    Vector2u textureSize = texturaJoc.getSize();
    Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    spriteJoc.setScale(scaleX, scaleY);
    spriteJoc.setPosition(0.f, 0.f);

    //Se seteaza titlul nivelului
    Text titlu;
    Text vietiText;
    vietiText.setFont(font);
    vietiText.setCharacterSize(50);
    vietiText.setFillColor(Color::Red);
    titlu.setFont(font);
    titlu.setString("Nivelul 1");
    titlu.setCharacterSize(150);
    titlu.setFillColor(Color::Yellow);
    titlu.setOutlineColor(Color::Black);
    titlu.setOutlineThickness(8.f);
    titlu.setPosition(600, 100);

    //Se seteaza mesajul in cazul in care jucatorul pierde
    bool isGameOver = false;
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(150);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setOutlineColor(Color::Black);
    gameOverText.setOutlineThickness(8.f);
    gameOverText.setPosition(600, 400);

    Clock gameOverClock;

    //Se seteaza timerele necesare
    Clock clock;
    Clock deltaClock;
    Clock titluClock;

    //Se declara vectorii de obiecte necesare pentru acest nivel
    float durataDisparitie = 1.f;
    vector<vector<Alien>> aliens;

    bool aliensInitialized = false;
    vector<Bullet> bullets;
    vector <Bullet_Racheta> bulletRachete;
    vector<Upgrade1> upgrades1;
    vector<ViataBonus> viataBonus;

    //Setez viata initiala a rachetei
    racheta.setVieti(3);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                return false; // Revino la meniu
            if (event.type == Event::MouseButtonPressed)
            {
                //Verific daca este apasat la mouse click stanga si trage cu un glont, doua, trei sau patru in functie de cate upgradeuri are racheta
                if (event.mouseButton.button == Mouse::Left)
                {
                    Vector2f rachetaPos = racheta.getPosition();
                    if(racheta.getUpgrade1() == 0)
                    bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                    else if (racheta.getUpgrade1() == 1) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                         bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(0.7f, -1.f));
                    }
                    else if (racheta.getUpgrade1() == 2) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(0.7f, -1.f));
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(-0.7f, -1.f));

                    }
                    else if (racheta.getUpgrade1() == 3) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 15 , rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 60 , rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 30, rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 45, rachetaPos.y + 50), bullet_racheta_texture);
                    }
                }
            }
        }
        float deltaTime = deltaClock.restart().asSeconds();

        // Calculeaza timpul scurs
        float timpTrecut = clock.getElapsedTime().asSeconds();

        //Face sa apara extraterestrii dupa doua secunde de la inceputul jocului ca sa aibe timp jucatorul sa isi retraga racheta in zona sigura
        if (!aliensInitialized && timpTrecut > 2.f) {
            initializeAliens(aliens, alien_texture, 1);
            aliensInitialized = true;
        }

        //Actualizeaza sansa de a trage a extraterestrului
        if (aliensInitialized) {
            updateShooting(clock, aliens, bullets, alien_bullet_texture, 1);
        }
        float alpha = 255 * (1.f - timpTrecut / durataDisparitie);
        if (alpha < 0) alpha = 0;

        titlu.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));

        //actualizeaza pozitia rachetei dupa pozitia mouse-ului
        Vector2i mousePos = Mouse::getPosition(window);
        racheta.update(mousePos);


        window.clear();
        window.draw(spriteJoc);  // Fundal nivel
        if (aliensInitialized) {
            for (auto& row : aliens) {
                for (auto& alien : row) {
                    alien.draw(window);
                }
            }
        }
        //Actualizeaza obiectele de pe ecran care se misca cum ar fi gloantele si bonusurile
        cadeUpgrade1(upgrades1, deltaTime, window);
        urcaGlontul(bulletRachete, deltaTime, window);
        cadeGlontul(bullets, deltaTime, window);
        cadeViata(viataBonus, deltaTime, window);
        // Verifica daca vreun glont extraterestru a lovit racheta si daca da atunci scade viata rachetei si sterge glontul
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
            if (bulletIt->getGlobalBounds().intersects(racheta.getGlobalBounds())) {
                racheta.setVieti(racheta.getVieti() - 1);
                if(racheta.getUpgrade1() > 0)
                    racheta.setUpgrade1(racheta.getUpgrade1() - 1);
                cout << "Racheta a fost lovita de glont extraterestru! Vieti ramase: " << racheta.getVieti() << endl;
                bulletIt = bullets.erase(bulletIt);
                if (racheta.getVieti() < 0) {
                    isGameOver = true;
                    gameOverClock.restart();
                }
            } else {
                ++bulletIt;
            }
        }

        //Varifica daca glontul rachetei a lovit extraterestrul, daca da atunci extraterestrul moare, este sters,
        //  si exista o sansa ca acel extraterestru sa dropeze un bonus
        for (auto bulletIt = bulletRachete.begin(); bulletIt != bulletRachete.end(); ) {
            bool bulletHit = false;

            for (auto rowIt = aliens.begin(); rowIt != aliens.end() && !bulletHit; ++rowIt) {
                for (auto alienIt = rowIt->begin(); alienIt != rowIt->end(); ) {
                    if (bulletIt->getGlobalBounds().intersects(alienIt->getGlobalBounds())) {
                        // Scade viața
                        alienIt->setviataAlien(alienIt->getviataAlien() - racheta.getDamage());

                        if (alienIt->getviataAlien() <= 0) {
                            static random_device rd;
                            static mt19937 gen(rd());
                            static uniform_real_distribution<> dis(0.0, 1.0);

                            if (dis(gen) <= 0.05f)
                                upgrades1.emplace_back(alienIt->getPosition(), upgrade1_texture);
                            if (dis(gen) <= 0.05f)
                                viataBonus.emplace_back(alienIt->getPosition(), texturaViataBonus);

                            alienIt = rowIt->erase(alienIt);
                        } else {
                            ++alienIt;
                        }

                        // Șterge glonțul
                        bulletIt = bulletRachete.erase(bulletIt);
                        bulletHit = true;
                        break;
                    } else {
                        ++alienIt;
                    }
                }
            }

            if (!bulletHit) {
                ++bulletIt;
            }
        }

        // Sterge randurile goale din clasa aliens
        for (auto rowIt = aliens.begin(); rowIt != aliens.end(); ) {
            if (rowIt->empty()) {
                rowIt = aliens.erase(rowIt);
            } else {
                ++rowIt;
            }
        }
        //Verific daca am omorat toti extraterestrii si daca da atunci dau return true si trec la nivelul 2
        if (aliensInitialized && aliens.empty()) {
            cout << "Ai omorat toti extraterestrii!" << endl;
            return true;
        }
        //Verifica daca vreun alien a lovit racheta, iar daca da racheta pierde o viata si extraterestrul dispare
        for (auto& row : aliens) {
            row.erase(remove_if(row.begin(), row.end(), [&](const Alien& alien) {
                if (racheta.getGlobalBounds().intersects(alien.getGlobalBounds())) {
                    racheta.setVieti(racheta.getVieti() - 1);
                    if(racheta.getUpgrade1() > 0)
                        racheta.setUpgrade1(racheta.getUpgrade1() - 1);
                    cout << "Racheta a fost lovita! Vieti ramase: " << racheta.getVieti() << endl;
                    if (racheta.getVieti() < 0) {
                          isGameOver = true;
                          gameOverClock.restart();
                     }
                    return true;
                }
                return false;
            }), row.end());
        }
        //verifica daca upgrade-ul a lovit racheta si daca se intampla asta creste upgrade-ul rachetei cu 1
        for (auto bulletIt = upgrades1.begin(); bulletIt != upgrades1.end(); ) {
            if (bulletIt->getGlobalBounds().intersects(racheta.getGlobalBounds())) {
                if(racheta.getUpgrade1() < 3)
                     racheta.setUpgrade1(racheta.getUpgrade1() + 1);
                cout << "Racheta a fost lovita de upgrade" << endl;
                bulletIt = upgrades1.erase(bulletIt);
            } else {
                ++bulletIt;
            }
        }
        //verifica daca viata a lovit racheta si daca da atunci racheta primeste o viata in plus
        for (auto bulletIt = viataBonus.begin(); bulletIt != viataBonus.end(); ) {
            if (bulletIt->getGlobalBounds().intersects(racheta.getGlobalBounds())) {
                    racheta.setVieti(racheta.getVieti() + 1);
                cout << "Racheta a fost lovita de viata" << endl;
                bulletIt = viataBonus.erase(bulletIt);
            } else {
                ++bulletIt;
            }
        }


        racheta.draw(window);        //deseneaza racheta
        //daca a trecut mai putin timp decat cel maxim pentru afisarea titlului atunci titlul continua sa fie afisat si in caz contrar nu mai e afisat
        if(titluClock.getElapsedTime().asSeconds() < durataDisparitie) {
            window.draw(titlu);
        }

        //Afiseaza vietile rachetei in coltul din dreapta jos
        vietiText.setString("Vieti: " + to_string(racheta.getVieti()));

        FloatRect textBounds = vietiText.getLocalBounds();
        float x = windowSize.x - textBounds.width - 20.f;
        float y = windowSize.y - textBounds.height - 20.f;
        vietiText.setPosition(x, y);
        racheta.draw(window);
        window.draw(vietiText);
        window.display();

        //Daca ai pierdut atunci nivelul returneaza false si te trimite in meniu
        if (isGameOver) {
            window.draw(gameOverText);
            window.display();

            if (gameOverClock.getElapsedTime().asSeconds() > 3.f) {
                return false;
            }
        }

    }


}

bool Nivel2(RenderWindow& window, Racheta& racheta) {
    //Seteaza asseturile necesare nivelului
    Texture texturaJoc;
    if (!texturaJoc.loadFromFile("assets/FundalNivel1.jpg"))
        return false;
    Texture texturaViataBonus;
    if(!texturaViataBonus.loadFromFile("assets/viata_bonus.png"))
        return false;
    Texture alien_texture;
    if(!alien_texture.loadFromFile("assets/extraterestru.png"))
        return false;

    Texture bullet_racheta_texture;
    if(!bullet_racheta_texture.loadFromFile("assets/glont_racheta.png"))
        return false;
    Texture alien_bullet_texture;
    if (!alien_bullet_texture.loadFromFile("assets/glont_alien.png"))
        return false;
    Texture upgrade1_texture;
    if (!upgrade1_texture.loadFromFile("assets/upgrade1.png"))
        return false;
    Font font;
    if (!font.loadFromFile("assets/FontulMeu.otf"))
        return false;
    // Seteaza fundalul jocului sa se scaleze automat pe toata fereastra
    Sprite spriteJoc;
    spriteJoc.setTexture(texturaJoc);
    Vector2u textureSize = texturaJoc.getSize();
    Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    spriteJoc.setScale(scaleX, scaleY);
    spriteJoc.setPosition(0.f, 0.f);

    //Seteaza titlul nivelului
    Text titlu;
    Text vietiText;
    vietiText.setFont(font);
    vietiText.setCharacterSize(50);
    vietiText.setFillColor(Color::Red);
    titlu.setFont(font);
    titlu.setString("Nivelul 2");
    titlu.setCharacterSize(150);
    titlu.setFillColor(Color::Yellow);
    titlu.setOutlineColor(Color::Black);
    titlu.setOutlineThickness(8.f);
    titlu.setPosition(600, 100);

    //Seteaza mesajul in caz de jucatorul pierde
    bool isGameOver = false;
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(150);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setOutlineColor(Color::Black);
    gameOverText.setOutlineThickness(8.f);
    gameOverText.setPosition(600, 400);

    Clock gameOverClock; // ca sa astepte putin inainte de a ajunge in meniu


    bool titluAfisat = false;
    Clock clock;
    Clock deltaClock;
    Clock titluClock;
    Clock waveClock;
    float durataDisparitie = 1.f;

    //Aici se seteaza extraterestrii pentru primul si al doilea val
    vector<vector<Alien>> aliens;
    vector<Alien> alienCircle;

    //se seteaza obiectele necesare
    int currentWave = 0;
    const int totalWaves = 2;
    bool aliensInitialized = false;
    vector<Bullet> bullets;
    vector <Bullet_Racheta> bulletRachete;
    vector<Upgrade1> upgrades1;
    vector<ViataBonus> viataBonus;



    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                return false;
            //Face sa traga racheta cand e apasat click stanga
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    Vector2f rachetaPos = racheta.getPosition();
                    if(racheta.getUpgrade1() == 0)
                    bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                    else if (racheta.getUpgrade1() == 1) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                         bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(0.7f, -1.f));
                    }
                    else if (racheta.getUpgrade1() == 2) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(0.7f, -1.f));
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(-0.7f, -1.f));

                    }
                    else if (racheta.getUpgrade1() == 3) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 15 , rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 60 , rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 30, rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 45, rachetaPos.y + 50), bullet_racheta_texture);
                    }
                }
            }
        }
        float deltaTime = deltaClock.restart().asSeconds();
        if (currentWave == 0) {
            //Actualizeaza extraterestrii care trag si cei de pe ecran
            updateShooting(clock, aliens, bullets, alien_bullet_texture, 1);
            updateAliens(aliens, deltaTime);
        } else if (currentWave == 1) {
            //Actualizeaza extraterestrii care trag
            updateShootingCircleAlien(clock, alienCircle, bullets, alien_bullet_texture, 1);
        }
        // Calculeaza timpul scurs
        float timpTrecut = clock.getElapsedTime().asSeconds();

        if (!aliensInitialized && waveClock.getElapsedTime().asSeconds() > 2.f) {
            aliens.clear();
            alienCircle.clear();

            if (currentWave == 0)
                initializeAliens(aliens, alien_texture, 2, AlienMovementType::STATIC);
            else if (currentWave == 1)
                initializeAliensCircle(alienCircle, alien_texture, 2, AlienMovementType::CIRCULAR);

            aliensInitialized = true;
        }

        //Actualizeaza miscarea extraterestriilor in functie de nivel
        if (currentWave == 0) {
            for (auto& row : aliens)
                for (auto& alien : row)
                    alien.update(deltaTime);
        } else if (currentWave == 1) {
            for (auto& alien : alienCircle)
                alien.update(deltaTime);
        }

        //Daca au fost initializati extraterestrii li se actualizeaza sansa de a trage
        if (aliensInitialized) {
            updateShooting(clock, aliens, bullets, alien_bullet_texture, 1);  // Trag glonțuri
        }

        float alpha = 255 * (1.f - timpTrecut / durataDisparitie);
        if (alpha < 0) alpha = 0;
        titlu.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));

        //Actualizeaza racheta in functie de pozitia mouse-ului
        Vector2i mousePos = Mouse::getPosition(window);
        racheta.update(mousePos);


        window.clear();
        window.draw(spriteJoc);  // Fundal nivel

        //Deseneaza matricea sau vectorul de extraterestrii in functie de wave
        if (aliensInitialized) {
            if (currentWave == 0) {
                for (auto& row : aliens) {
                    for (auto& alien : row) {
                        alien.draw(window);
                    }
                }
            } else if (currentWave == 1) {
                for (auto& alien : alienCircle) {
                    alien.draw(window);
                }
            }
        }

        //Aici se actualizeaza obiectele care se misca pe ecran
        cadeUpgrade1(upgrades1, deltaTime, window);
        urcaGlontul(bulletRachete, deltaTime, window);
        cadeGlontul(bullets, deltaTime, window);
        cadeViata(viataBonus, deltaTime, window);
        // Verifica daca vreun glont extraterestru a lovit racheta si daca da scade viata la racheta
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
            if (bulletIt->getGlobalBounds().intersects(racheta.getGlobalBounds())) {
                racheta.setVieti(racheta.getVieti() - 1);
                if(racheta.getUpgrade1() > 0)
                    racheta.setUpgrade1(racheta.getUpgrade1() - 1);
                cout << "Racheta a fost lovita de glont extraterestru! Vieti ramase: " << racheta.getVieti() << endl;
                bulletIt = bullets.erase(bulletIt);
                if (racheta.getVieti() < 0) {
                    isGameOver = true;
                    gameOverClock.restart();
                }
            } else {
                ++bulletIt;
            }aliens.empty();
        }

        //Verifica glontul rachetei daca loveste extraterestrii din primul val si daca sunt loviti verifica daca trebuie sa dropeze un bonus
        for (auto bulletIt = bulletRachete.begin(); bulletIt != bulletRachete.end(); ) {
            bool bulletHit = false;

            for (auto rowIt = aliens.begin(); rowIt != aliens.end() && !bulletHit; ++rowIt) {
                for (auto alienIt = rowIt->begin(); alienIt != rowIt->end(); ) {
                    if (bulletIt->getGlobalBounds().intersects(alienIt->getGlobalBounds())) {
                        // Scade viata
                        alienIt->setviataAlien(alienIt->getviataAlien() - racheta.getDamage());

                        // Daca extaterestrul trebuie sa moara
                        if (alienIt->getviataAlien() <= 0) {
                            // Drop random de upgrade sau viata
                            static random_device rd;
                            static mt19937 gen(rd());
                            static uniform_real_distribution<> dis(0.0, 1.0);

                            if (dis(gen) <= 0.03f)
                                upgrades1.emplace_back(alienIt->getPosition(), upgrade1_texture);
                            if (dis(gen) <= 0.01f)
                                viataBonus.emplace_back(alienIt->getPosition(), texturaViataBonus);

                            // Sterge extraterestrul
                            alienIt = rowIt->erase(alienIt);
                        } else {
                            ++alienIt;
                        }

                        // Șterge glonțul
                        bulletIt = bulletRachete.erase(bulletIt);
                        bulletHit = true;
                        break;
                    } else {
                        ++alienIt;
                    }
                }
            }

            // Daca nu a lovit nimic merge la urmatorul glont
            if (!bulletHit) {
                ++bulletIt;
            }
        }
        //verifica daca un bullet de la racheta loveste extraterestrii din al doilea val si daca da omoara estraterestrul la doua lovituri si verifica daca va dropa un bonus
        if (currentWave == 1) {
            for (auto bulletIt = bulletRachete.begin(); bulletIt != bulletRachete.end(); ) {
                bool bulletHit = false;

                for (auto alienIt = alienCircle.begin(); alienIt != alienCircle.end(); ) {
                    if (bulletIt->getGlobalBounds().intersects(alienIt->getGlobalBounds())) {
                        alienIt->setviataAlien(alienIt->getviataAlien() - racheta.getDamage());

                        if (alienIt->getviataAlien() <= 0) {
                            // drop
                            static random_device rd;
                            static mt19937 gen(rd());
                            static uniform_real_distribution<> dis(0.0, 1.0);

                            if (dis(gen) <= 0.03f)
                                upgrades1.emplace_back(alienIt->getPosition(), upgrade1_texture);
                            if (dis(gen) <= 0.01f)
                                viataBonus.emplace_back(alienIt->getPosition(), texturaViataBonus);

                            alienIt = alienCircle.erase(alienIt);
                        } else {
                            ++alienIt;
                        }

                        bulletIt = bulletRachete.erase(bulletIt);
                        bulletHit = true;
                        break;
                    } else {
                        ++alienIt;
                    }
                }

                if (!bulletHit)
                    ++bulletIt;
            }
        }


        // Sterge randurile goale din extraterestrii
        for (auto rowIt = aliens.begin(); rowIt != aliens.end(); ) {
            if (rowIt->empty()) {
                rowIt = aliens.erase(rowIt);
            } else {
                ++rowIt;
            }
        }

        // Verifica daca un extraterestru a lovit racheta
        for (auto& row : aliens) {
            row.erase(remove_if(row.begin(), row.end(), [&](const Alien& alien) {
                if (racheta.getGlobalBounds().intersects(alien.getGlobalBounds())) {
                    racheta.setVieti(racheta.getVieti() - 1);
                    if(racheta.getUpgrade1() > 0)
                        racheta.setUpgrade1(racheta.getUpgrade1() - 1);
                    cout << "Racheta a fost lovita! Vieti ramase: " << racheta.getVieti() << endl;
                    if (racheta.getVieti() < 0) {
                          isGameOver = true;
                          gameOverClock.restart();
                     }
                    return true;
                }
                return false;
            }), row.end());
        }
        //Verifica daca un extraterestru din al doilea val s-a lovit cu racheta si daca da scade viata rachetei si extraterestrul dispare
        alienCircle.erase(remove_if(alienCircle.begin(), alienCircle.end(), [&](const Alien& alien) {
                if (racheta.getGlobalBounds().intersects(alien.getGlobalBounds())) {
                    racheta.setVieti(racheta.getVieti() - 1);
                     if (racheta.getUpgrade1() > 0)
                         racheta.setUpgrade1(racheta.getUpgrade1() - 1);
                     cout << "Racheta a fost lovita de un alien circular! Vieti ramase: " << racheta.getVieti() << endl;

                      if (racheta.getVieti() < 0) {
                           isGameOver = true;
                           gameOverClock.restart();
                        }
                    return true;
                    }
                return false;
            }), alienCircle.end());
        // Verifica daca upgrade-ul a lovit racheta
        for (auto bulletIt = upgrades1.begin(); bulletIt != upgrades1.end(); ) {
            if (bulletIt->getGlobalBounds().intersects(racheta.getGlobalBounds())) {
                if(racheta.getUpgrade1() < 3)
                     racheta.setUpgrade1(racheta.getUpgrade1() + 1);
                cout << "Racheta a fost lovita de upgrade" << endl;
                bulletIt = upgrades1.erase(bulletIt);
            } else {
                ++bulletIt;
            }
        }
        // Verifica daca viata a lovit racheta
        for (auto bulletIt = viataBonus.begin(); bulletIt != viataBonus.end(); ) {
            if (bulletIt->getGlobalBounds().intersects(racheta.getGlobalBounds())) {
                    racheta.setVieti(racheta.getVieti() + 1);
                cout << "Racheta a fost lovita de viata" << endl;
                bulletIt = viataBonus.erase(bulletIt);
            } else {
                ++bulletIt;
            }
        }


        racheta.draw(window);   //afiseaza racheta
        //Verifica sa se afiseze numai primele secunde din nivel titlul
        if (!titluAfisat && titluClock.getElapsedTime().asSeconds() < durataDisparitie) {
            window.draw(titlu);
        } else {
            titluAfisat = true;
        }
        vietiText.setString("Vieti: " + to_string(racheta.getVieti()));    //seteaza stringul care va afisa viata rachetei
        //Afiseaza vietile rachetei in coltul din dreapta jos
        FloatRect textBounds = vietiText.getLocalBounds();
        float x = windowSize.x - textBounds.width - 20.f;
        float y = windowSize.y - textBounds.height - 20.f;
        vietiText.setPosition(x, y);
        racheta.draw(window);
        window.draw(vietiText);
        window.display();
        //verifica daca jucatorul a pierdut si il duce in meniu daca a pierdut
        if (isGameOver) {
            window.draw(gameOverText);
            window.display();

            if (gameOverClock.getElapsedTime().asSeconds() > 3.f) {
                return false;
            }
        }
        //verifica daca a omorat racheta toti extraterestrii si daca da trece la urmatorul nivel
        if (aliensInitialized && ((currentWave == 0 && aliens.empty()) || (currentWave == 1 && alienCircle.empty()))) {

            if (currentWave < totalWaves - 1) {
                currentWave++;
                aliensInitialized = false;
                waveClock.restart();
                titluClock.restart();
            } else {
                cout << "Ai omorat toti extraterestrii din toate valurile!" << endl;
                return true;
            }
        }


    }
}

void Nivel3(RenderWindow& window, Racheta& racheta) {
    //Incarc texturile pentru joc si fontul
    Texture texturaArenaBoss;
    if (!texturaArenaBoss.loadFromFile("assets/boss_backround.png"))
        return;
    Texture bullet_racheta_texture;
    if(!bullet_racheta_texture.loadFromFile("assets/glont_racheta.png"))
        return;
    Texture alien_bullet_texture;
    if (!alien_bullet_texture.loadFromFile("assets/glont_alien.png"))
        return;
    Texture textura_boss;
    if(!textura_boss.loadFromFile("assets/poza_boss.png"))
        return;
    Font font;
    if(!font.loadFromFile("assets/FontulMeu.otf"))
        return;

    // Seteaza fundalul jocului sa se scaleze automat pe toata fereastra
    Sprite spriteJoc;
    spriteJoc.setTexture(texturaArenaBoss);
    Vector2u textureSize = texturaArenaBoss.getSize();
    Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    spriteJoc.setScale(scaleX, scaleY);
    spriteJoc.setPosition(0.f, 0.f);

    //Seteaza titlul nivelului
    Text titlu;
    Text vietiText;
    vietiText.setFont(font);
    vietiText.setCharacterSize(50);
    vietiText.setFillColor(Color::Red);
    titlu.setFont(font);
    titlu.setString("Boss Fight");
    titlu.setCharacterSize(150);
    titlu.setFillColor(Color::Yellow);
    titlu.setOutlineColor(Color::Black);
    titlu.setOutlineThickness(8.f);
    titlu.setPosition(600, 100);

    //Seteaza mesajul in caz de pierdere
    bool isGameOver = false;
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(150);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setOutlineColor(Color::Black);
    gameOverText.setOutlineThickness(8.f);
    gameOverText.setPosition(600, 400);

    //Aici se seteaza cronometrele
    bool titluAfisat = false;
    Clock clock;
    Clock deltaClock;
    Clock titluClock;
    float durataDisparitie = 1.f;

    //Se seteaza clasele
    vector<BulletBoss> bullets;
    vector <Bullet_Racheta> bulletRachete;
    RomanianFinalBoss Boss(Vector2f(600.f, 50.f), textura_boss);

    //Se seteaza textul cu viata bossului
    Text bossViataText;
    bossViataText.setFont(font);
    bossViataText.setCharacterSize(40);
    bossViataText.setFillColor(Color::White);
    bossViataText.setString("Boss: " + to_string(Boss.getviataBoss()));
    bossViataText.setPosition(20.f, 20.f);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                return;
            //Se verifica cand este apasat click stanga ca sa traga racheta si in functie de upgrade poate sa traga cu mai multe gloante
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    Vector2f rachetaPos = racheta.getPosition();
                    if(racheta.getUpgrade1() == 0)
                    bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                    else if (racheta.getUpgrade1() == 1) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                         bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(0.7f, -1.f));
                    }
                    else if (racheta.getUpgrade1() == 2) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(0.7f, -1.f));
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), bullet_racheta_texture, Vector2f(-0.7f, -1.f));

                    }
                    else if (racheta.getUpgrade1() == 3) {
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 15 , rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 60 , rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 30, rachetaPos.y + 50), bullet_racheta_texture);
                        bulletRachete.emplace_back(Vector2f(rachetaPos.x + 45, rachetaPos.y + 50), bullet_racheta_texture);
                    }
                }
            }
        }
        float deltaTime = deltaClock.restart().asSeconds();
        float timpTrecut = clock.getElapsedTime().asSeconds();


        float alpha = 255 * (1.f - timpTrecut / durataDisparitie);
        if (alpha < 0) alpha = 0;

        titlu.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));


        Vector2i mousePos = Mouse::getPosition(window);
        racheta.update(mousePos);


        window.clear();
        window.draw(spriteJoc);

        Vector2f pozRacheta = racheta.getPosition();
        Boss.update(deltaTime, window, pozRacheta);     //Actualizeaza pozitia Boss-ului in timp

        //Actualizeaza viata Boss-ului cand e lovit de glontul rachetei
        for (auto it = bulletRachete.begin(); it != bulletRachete.end(); ) {
            if (it->getGlobalBounds().intersects(Boss.getGlobalBounds())) {
                Boss.takeDamage(racheta.getDamage() * 5);
                bossViataText.setString("Boss: " + to_string(Boss.getviataBoss()));
                it = bulletRachete.erase(it);
            } else {
                ++it;
            }
        }

        auto maybeDir = Boss.fire();          //Face Boss-ul sa traga
        if (maybeDir.has_value()) {
            bullets.emplace_back(Boss.getPosition(), alien_bullet_texture, maybeDir.value());   //salveaza gloantele Boss-ului care sunt in ecran
        }


        urcaGlontul(bulletRachete, deltaTime, window);               //Misca glontul rachetei
        cadeGlontulBoss(bullets, deltaTime, window, racheta);     //Actualizeaza glontul Boss-ului


        racheta.draw(window);
        if (!titluAfisat && titluClock.getElapsedTime().asSeconds() < durataDisparitie) {
            window.draw(titlu);
        } else {
            titluAfisat = true;
        }
        vietiText.setString("Vieti: " + to_string(racheta.getVieti()));

        //Seteaza locatia in care sunt afisate vietile rachetei
        FloatRect textBounds = vietiText.getLocalBounds();
        float x = windowSize.x - textBounds.width - 20.f;
        float y = windowSize.y - textBounds.height - 20.f;
        vietiText.setPosition(x, y);
        racheta.draw(window);
        window.draw(vietiText);

        //se verifica daca a fost invins boss-ul
        if (Boss.getviataBoss() <= 0) {
            cout << "Boss-ul a fost invins!\n";
            return;
        }
        // Verifica daca racheta s-a ciocnit direct cu bossul
        if (racheta.getGlobalBounds().intersects(Boss.getGlobalBounds()) || racheta.getVieti() <= 0) {
            racheta.setVieti(0);  // pierzi toate viețile instant
            isGameOver = true;
        }
        //Verifica daca ai pierdut
        if (isGameOver) {
            window.draw(gameOverText);
            window.display();
            sleep(seconds(2));
            return;
        }

        window.draw(bossViataText);        //deseneaza viata boss-ului
        Boss.draw(window);              // deseneaza boss-ul
        window.display();



    }
}


//aici accesez fiecare nivel pe rand daca reusesc sa trec de toate si daca nu ma scoate din functie
void GameLoop(RenderWindow& window) {
    Texture racheta_textura;
    if(!racheta_textura.loadFromFile("assets/racheta.png"))
        return;

    Racheta racheta(racheta_textura);
    bool isGameOver = Jocul(window, racheta);   // Nivel 1

    if (!isGameOver)
        return;

    bool nivel2Passed = Nivel2(window, racheta);  // ⇽ primește bool

    if (!nivel2Passed)
        return;

    Nivel3(window, racheta);  // ⇽ se intră doar dacă Nivel2 s-a trecut cu succes
}

int main() {
    RenderWindow window(VideoMode(1980, 1080), "Meniu cu Mouse");

    Font font;

    Texture TexturaDeFundal;

    if(!TexturaDeFundal.loadFromFile("assets/meniu.png")) {
        return -1;
    }
    //setez fundalul jocului
    Sprite SpriteDeFundal;
    SpriteDeFundal.setTexture(TexturaDeFundal);
    SpriteDeFundal.setPosition(450.f, 5.f);
    SpriteDeFundal.setScale(Vector2(2.f, 2.f));

    if (!font.loadFromFile("assets/FontulMeu.otf")) {
        cerr << "Nu s-a putut incarca fontul!" << endl;
        return -1;
    }
    //creez cele 3 optiuni in meniu
    const int NUM_OPTIONS = 3;
    Text menu[NUM_OPTIONS];
    string options[] = { "Start Joc", "Setari", "Iesire" };

    for (int i = 0; i < NUM_OPTIONS; ++i) {
        menu[i].setFont(font);
        menu[i].setString(options[i]);
        menu[i].setCharacterSize(200);
        menu[i].setPosition(700, 150 + i * 200);
        menu[i].setFillColor(Color::White);
    }


    while (window.isOpen()) {
        Event event;
        Vector2i mousePos = Mouse::getPosition(window);

        for (int i = 0; i < NUM_OPTIONS; ++i) {
            if (menu[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                menu[i].setFillColor(Color::Red);
                //daca apas incepe jocul incepe primul nivel, daca apas setari nu se intampla nimic momentan si daca apas iesire jocul se inchide
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    if(i == 0) {
                        cout << "incepe jocul" << endl;
                        GameLoop(window);
                    }
                    else if(i == 1) {
                        cout << "setari" << endl;
                    }
                    else if(i == 2) {
                        window.close();
                    }
                }

            } else {
                menu[i].setFillColor(Color::White);
            }
        }

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(SpriteDeFundal);
        for (int i = 0; i < NUM_OPTIONS; ++i)
            window.draw(menu[i]);
        window.display();
    }

    return 0;
}