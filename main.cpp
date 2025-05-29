#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include <string>
#include <random>
#include <vector>

using namespace std;
using namespace sf;

class Alien {
private:
    Sprite sprite;
    Texture texture;
    bool isShooting = false;

public:
    Alien(const Vector2f& position, const string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            cout << "nu merge textura extraterestrului" << endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(position);
    }

    void draw(RenderWindow& window) {
        window.draw(sprite);
    }

    bool tryToShoot() {
        static random_device rd;
        static mt19937 gen(rd());
        static uniform_real_distribution<> dis(0.0, 1.0);
        float chance = dis(gen);
        return chance <= 0.2f;
    }
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
    Texture texture;
    float speed = 300.f;
public:
    Bullet_Racheta(const Vector2f& startPos, const string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            cerr << "Nu s-a putut încarca textura glontului rachetei!\n";
        }
        sprite.setTexture(texture);
        sprite.setPosition(startPos);
    }

    void update(float deltaTime) {
        sprite.move(0.f, speed * deltaTime * (-1));  // Misca glontul în sus
    }

    bool isOffScreen(float screenHeight) const {
        return sprite.getPosition().y > screenHeight;
    }


    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

};

class Bullet {
private:
    Sprite sprite;
    Texture texture;
    float speed = 200.f;
public:
    Bullet(const Vector2f& startPos, const string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            cerr << "Nu s-a putut incarca textura glontului!\n";
        }
        sprite.setTexture(texture);
        sprite.setPosition(startPos);
    }

    void update(float deltaTime) {
        sprite.move(0.f, speed * deltaTime);  // Misca glontul în jos
    }

    bool isOffScreen(float screenHeight) const {
        return sprite.getPosition().y > screenHeight;
    }

    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

};

class Racheta {
private:
    Sprite sprite;
    Texture texture;
    int mVieti;

public:
    Racheta(const string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            cerr << "Eroare la incarcarea texturii!" << endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(960, 900);
    }

    void update(Vector2i mousePos) {
        sprite.setPosition(mousePos.x - sprite.getGlobalBounds().width / 2, mousePos.y - sprite.getGlobalBounds().height / 2);
    }

    void draw(RenderWindow& window) {
        window.draw(sprite);
    }

    void setVieti(int vieti) {
        mVieti = vieti;
    }

    int getVieti() const {
        return mVieti;
    }
    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }


    Vector2f getPosition() const {
        return sprite.getPosition();
    }

    bool shoot() {

    }
};

void urcaGlontul(vector<Bullet_Racheta>& bullets, float deltaTime, RenderWindow& window) {
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
        bullet.draw(window);
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(), [&window](const Bullet_Racheta& b) {
            return b.isOffScreen(window.getSize().y);
        }), bullets.end());
}

void cadeGlontul(vector<Bullet>& bullets, float deltaTime, RenderWindow& window) {
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
        bullet.draw(window);
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(), [&window](const Bullet& b) {
            return b.isOffScreen(window.getSize().y);
        }), bullets.end());
}

void initializeAliens(vector<vector<Alien>>& aliens) {
    for (int row = 0; row < 10; ++row) {
        vector<Alien> alienRow;
        for (int col = 0; col < 18; ++col) {
            Vector2f pos(100 + col * 100, 70 + row * 70);
            Alien alien(pos, "/home/ruk/CLionProjects/untitled/simion_cel_mic.jpg");
            alienRow.push_back(alien);
        }
        aliens.push_back(alienRow);
    }
}


void updateShooting(Clock& shootClock, vector<vector<Alien>>& aliens, vector<Bullet>& bullets) {
    if (shootClock.getElapsedTime().asSeconds() >= 5) {
        for (auto& row : aliens) {
            for (auto& alien : row) {
                if (alien.tryToShoot()) {
                    // Creezi un glonț sau marchezi că trage
                    Vector2f alienPos = alien.getPosition();
                    Bullet newBullet(Vector2f(alienPos.x + 25, alienPos.y + 50), "/home/ruk/CLionProjects/untitled/glont_de_monstru.png");
                    bullets.push_back(newBullet);
                }
            }
        }
        shootClock.restart();
    }
}

void Jocul(RenderWindow& window) {
    Texture texturaJoc;
    if (!texturaJoc.loadFromFile("/home/ruk/CLionProjects/untitled/FundalNivel1.jpg"))
        return;
    Sprite spriteJoc;
    spriteJoc.setTexture(texturaJoc);
    Vector2u textureSize = texturaJoc.getSize();
    Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    spriteJoc.setScale(scaleX, scaleY);
    spriteJoc.setPosition(0.f, 0.f);
    Font font;
    if (!font.loadFromFile("/home/ruk/CLionProjects/untitled/FontulMeu.otf"))
        return;

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


    Clock clock;
    Clock deltaClock;
    Clock titluClock;
    float durataDisparitie = 1.f;
    vector<vector<Alien>> aliens;

    bool aliensInitialized = false;
    vector<Bullet> bullets;
    vector <Bullet_Racheta> bulletRachete;

    Racheta racheta("/home/ruk/CLionProjects/untitled/racheta.jpg");
    racheta.setVieti(3);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                return; // Revino la meniu
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    Vector2f rachetaPos = racheta.getPosition();
                    Bullet_Racheta newBullet(Vector2f(rachetaPos.x + 25, rachetaPos.y + 50), "/home/ruk/CLionProjects/untitled/glont_de_monstru.png");
                    bulletRachete.push_back(newBullet);
                }
            }
        }
        float deltaTime = deltaClock.restart().asSeconds();
        if (!aliens.empty())
            updateShooting(clock, aliens, bullets);
        // Calculeaza timpul scurs
        float timpTrecut = clock.getElapsedTime().asSeconds();

        if (!aliensInitialized && timpTrecut > 2.f) {
            initializeAliens(aliens);
            aliensInitialized = true;
        }

        if (aliensInitialized) {
            updateShooting(clock, aliens, bullets);  // Trag glonțuri
        }
        float alpha = 255 * (1.f - timpTrecut / durataDisparitie);
        if (alpha < 0) alpha = 0;

        titlu.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));



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
        urcaGlontul(bulletRachete, deltaTime, window);
        cadeGlontul(bullets, deltaTime, window);
        for (auto it = bullets.begin(); it != bullets.end();) {
            if (racheta.getGlobalBounds().intersects(it->getGlobalBounds())) {
                cout << "Racheta a fost lovita!\n";
                it = bullets.erase(it);
                racheta.setVieti(racheta.getVieti() - 1);
            } else {
                ++it;
            }
        }
        for (auto rowIt = aliens.begin(); rowIt != aliens.end(); ) {
            bool aLovitRacheta = false;
            rowIt->erase(remove_if(rowIt->begin(), rowIt->end(), [&](const Alien& alien) {
                if (racheta.getGlobalBounds().intersects(alien.getGlobalBounds())) {
                    aLovitRacheta = true;
                    return true;
                }
                return false;
            }), rowIt->end());
            if (!aliens.empty()) {
                for (auto bulletIt = bulletRachete.begin(); bulletIt != bulletRachete.end(); ) {
                    bool bulletHit = false;

                    for (auto rowIt = aliens.begin(); rowIt != aliens.end() && !bulletHit; ) {
                        for (auto alienIt = rowIt->begin(); alienIt != rowIt->end(); ) {
                            if (bulletIt->getGlobalBounds().intersects(alienIt->getGlobalBounds())) {
                                alienIt = rowIt->erase(alienIt);
                                bulletIt = bulletRachete.erase(bulletIt);
                                bulletHit = true;
                                break;
                            } else {
                                ++alienIt;
                            }
                        }

                        if (rowIt->empty()) {
                            rowIt = aliens.erase(rowIt);
                        } else {
                            ++rowIt;
                        }
                    }

                    if (!bulletHit) {
                        ++bulletIt;
                    }
                }
            }

            if (aLovitRacheta) {
                racheta.setVieti(racheta.getVieti() - 1);
                cout << "Racheta a fost lovita! Vieți rămase: " << racheta.getVieti() << endl;
            }

            if (rowIt->empty()) {
                rowIt = aliens.erase(rowIt);
            } else {
                ++rowIt;
            }
        }
        racheta.draw(window);
        if(titluClock.getElapsedTime().asSeconds() < durataDisparitie) {
            window.draw(titlu);
        }
        vietiText.setString("Vieti: " + to_string(racheta.getVieti()));

        FloatRect textBounds = vietiText.getLocalBounds();
        float x = windowSize.x - textBounds.width - 20.f;
        float y = windowSize.y - textBounds.height - 20.f;
        vietiText.setPosition(x, y);

        window.draw(vietiText);
        window.display();
    }


}

int main() {
    RenderWindow window(VideoMode(1980, 1080), "Meniu cu Mouse");

    Font font;

    Texture TexturaDeFundal;

    if(!TexturaDeFundal.loadFromFile("/home/ruk/CLionProjects/untitled/Fundal.jpg")) {
        return -1;
    }

    Sprite SpriteDeFundal;
    SpriteDeFundal.setTexture(TexturaDeFundal);
    SpriteDeFundal.setPosition(450.f, 5.f);
    SpriteDeFundal.setScale(Vector2(2.f, 2.f));

    if (!font.loadFromFile("/home/ruk/CLionProjects/untitled/FontulMeu.otf")) {
        cerr << "Nu s-a putut incarca fontul!" << endl;
        return -1;
    }

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

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    if(i == 0) {
                        cout << "incepe jocul" << endl;
                        Jocul(window);
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