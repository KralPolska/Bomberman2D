#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include <Windows.h>

/**
    * @file
    **/
///Podstawowe wartości używane w całym skrypcie
const float FPS = 60.0;

#define MAX_BOMBS 10

#define PLAYER_SIZE 48
#define TILE_SIZE 64
#define MAP_HEIGHT 13
#define MAP_WIDTH 15

#define DISPLAY_HEIGHT TILE_SIZE*MAP_HEIGHT
#define DISPLAY_WIDTH TILE_SIZE*MAP_WIDTH

typedef int tab[MAP_HEIGHT][MAP_WIDTH];

///Tablica z poziomem do resetowania [[TYMCZASOWE, ZAMIENIĆ NA PLIK]] [ZAMIENIONE] {jako tako xD}
tab map_template = { 0 };

///Tablica z poziomem która zmienia się dynamicznie
tab map_table = { 0 };

/// load_map_table
/// \brief Funkcja która ładuje mapę poziomu z pliku do tablicy map_template
/// \param level_selected Wybrany numer poziomu z menu
/// \return Odległość początku od końca
void load_map_table(short level_selected)
{
    short x = 0;
    FILE* wsk_file = NULL;
    errno_t errorCheck;

    switch (level_selected)
    {
    case 0: errorCheck = fopen_s(&wsk_file, "poziom0.txt", "r"); break;
    case 1: errorCheck = fopen_s(&wsk_file, "poziom1.txt", "r"); break;
    case 2: errorCheck = fopen_s(&wsk_file, "poziom2.txt", "r"); break;
    case 3: errorCheck = fopen_s(&wsk_file, "poziom3.txt", "r"); break;
    case 4: errorCheck = fopen_s(&wsk_file, "poziom4.txt", "r"); break;
    default:errorCheck = fopen_s(&wsk_file, "poziom0.txt", "r"); break;
    }
    if (errorCheck != 0)
    {
        exit(1);
    }
    else
    {
        int i, j;
        for (i= 0; i < MAP_HEIGHT; i++)
        {
            for (j = 0; j < MAP_WIDTH; j++)
            {
                fscanf_s(wsk_file, "%d", &map_template[i][j]);
                printf("%d\t", map_template[i][j]);
            }
            puts("");
        }
    }
    fclose(wsk_file);
}

/// load_map
/// \brief Funkcja która ładuje wszystkie bitmapy do graficznego przedstawienia poziomu
/// \param map Tablica danych mapy
/// \param bircks Bitmapa z teksturą ścian stałych
/// \param boxes Bitmapa z teksturą ściań do niszczenia
/// \param center Bitmapa z teksturą środka wybuchu bomby
/// \param vertical Bitmapa z teksturą pionową wybuchu bomby
/// \param horizontal Bitmapa z teksturą poziomą wybuchu bomby
/// \param top Bitmapa z teksturą górnego końca wybuchu bomby
/// \param bottom Bitmapa z teksturą dolnego końca wybuchu bomby
/// \param left Bitmapa z teksturą lewego końca wybuchu bomby
/// \param right Bitmapa z teksturą prawego końca wybuchu bomby
/// \param fire Bitmapa z teksturą ulepszenia fireUp
/// \param bomb Bitmapa z teksturą ulepszenia bombUp
/// \param speed Bitmapa z teksturą ulepszenia speedUp
void load_map(tab map, ALLEGRO_BITMAP* bricks, ALLEGRO_BITMAP* boxes, 
    ALLEGRO_BITMAP* center, ALLEGRO_BITMAP* vertical, ALLEGRO_BITMAP* horizontal, 
    ALLEGRO_BITMAP* top, ALLEGRO_BITMAP* bottom, ALLEGRO_BITMAP* left, ALLEGRO_BITMAP* right, 
    ALLEGRO_BITMAP* fire, ALLEGRO_BITMAP* bomb, ALLEGRO_BITMAP* speed, 
    ALLEGRO_BITMAP* light_shape, ALLEGRO_BITMAP* light_map, ALLEGRO_DISPLAY* display) {
    int x, y;

    for (y = 0; y < MAP_WIDTH; y++) {
        for (x = 0; x < MAP_HEIGHT; x++) {
            int tile_index = map[x][y];
            int tile_x = tile_index / MAP_WIDTH;
            int tile_y = tile_index / MAP_HEIGHT;

            /// Kolizje
            if (tile_index == 11) {
                al_draw_bitmap(bricks, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 12) {
                al_draw_bitmap(boxes, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            /// Wybuch
            if (tile_index == 13) {
                al_draw_bitmap(center, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 14) {
                al_draw_bitmap(vertical, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 15) {
                al_draw_bitmap(horizontal, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 16) {
                al_draw_bitmap(top, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 17) {
                al_draw_bitmap(bottom, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 18) {
                al_draw_bitmap(left, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 19) {
                al_draw_bitmap(right, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            /// Ulepszenia
            if (tile_index == 20) {
                al_draw_bitmap(fire, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 21) {
                al_draw_bitmap(bomb, y * TILE_SIZE, x * TILE_SIZE, 0);
            }
            if (tile_index == 22) {
                al_draw_bitmap(speed, y * TILE_SIZE, x * TILE_SIZE, 0);
            }

            if (tile_index >= 13 && tile_index <= 19) {
                al_set_target_bitmap(light_map);
                al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

                al_draw_tinted_bitmap(light_shape, al_map_rgba_f(255, 255, 0, 0.5), y*TILE_SIZE-250, x*TILE_SIZE-250, 0);

                al_set_target_bitmap(al_get_backbuffer(display));
            }
        }
    }
}

/// \struct Character
/// \brief Struktura danych przedstawiająca postać
struct Character {
    int x; ///< Pozycja X postaci
    int y; ///< Pozycja Y postaci
    short speed; ///< Prędkość
    short bombs; ///< Maksymalna ilość bomb
    short power; ///< Siła wybuchu
    short direction; ///< Kierunek poruszania się
    ALLEGRO_BITMAP* image_dead; ///< Bitmapa z teksturą martwej postaci
    ALLEGRO_BITMAP* spritesheet; ///< Bitmapa z teksturami animacji postaci
};

/// create_character
/// \brief Funkcja do tworzenia domyślnej postaci ("konstruktor" swego rodzaju)
/// \param image_dead Bitmapa z teksturą martwej postaci
/// \param spritesheet Bitmapa z teksturami animacji postaci
/// \param x Pozycja X postaci
/// \param y Pozycja Y postaci
/// \param speed Prędkość postaci
/// \param bombs Maksymalna ilość bomb możliwych do postawienia jednocześnie
/// \param power Siła wybuchu
/// \return Struktura postaci z odpowiednimi wartościami
struct Character create_character(ALLEGRO_BITMAP* image_dead, ALLEGRO_BITMAP* spritesheet, int x, int y, int speed, int bombs, int power) {
    struct Character character;
    character.x = x;
    character.y = y;
    character.speed = speed;
    character.bombs = bombs;
    character.power = power;
    character.direction = 1;
    character.image_dead = image_dead;
    character.spritesheet = spritesheet;
    return character;
}

/// power_up
/// \brief Funkcja losująca jedno z trzech możliwych ulepszeń na pozycji zniszczonej skrzynki. Jest to 20%.
/// \param x Pozycja X skrzynki
/// \param y pozycja Y skrzynki
void power_up(int x, int y) {
    srand(time(NULL) + x * y);

    int chance = rand() % 5;
    ///printf("%d",chance);
    if (chance == 1) {
        map_table[y][x] = 20 + rand() % 3;
    }
}

/// power_up_collect
/// \brief Funkcja zmieniająca dane postaci na podstawie podniesionego power 
/// \param character Struktura danych postaci
/// \param type Typ podniesionego ulepszenia
void power_up_collect(struct Character* character, int type) {
    if (type == 20) {
        if (character->power < 16) {
            character->power += 1;
        }
    }
    if (type == 21) {
        if (character->bombs < MAX_BOMBS) {
            character->bombs += 1;
        }
    }
    if (type == 22) {
        if (character->speed < 7) {
            character->speed += 1;
        }
    }
}

/// check_collision
/// \brief Funkcja sprawdzająca kolizję z blokami stałymi, wybuchem bomby oraz ulepszeniami
/// \param character Struktura danych postaci 
/// \param dx Pozycja X gracza
/// \param dy Pozycja Y gracza
/// \param timer Timer na którym działa główna pętla
/// \param player_kill_sound Instacja pliku dźwiękowego w sytuacji, gdy postać umiera
/// \param collect_sound Instacja pliku dźwiękowego w sytuacji, gdy postać podnosi ulepszenie
/// \return Wartość prawda lub fałsz w postaci int
int check_collision(struct Character* character, int dx, int dy, ALLEGRO_TIMER* timer, ALLEGRO_SAMPLE_INSTANCE* player_kill_sound, ALLEGRO_SAMPLE_INSTANCE* collect_sound) {
    int new_x = character->x + dx;
    int new_y = character->y + dy;

    int left = new_x;
    int right = new_x + 48;
    int top = new_y;
    int bottom = new_y + 48;

    int tile_left = left / TILE_SIZE;
    int tile_right = right / TILE_SIZE;
    int tile_top = top / TILE_SIZE;
    int tile_bottom = bottom / TILE_SIZE;

    for (int y = tile_top; y <= tile_bottom; y++) {
        for (int x = tile_left; x <= tile_right; x++) {
            if (map_table[y][x] >= 13 && map_table[y][x] <= 19) {
                al_set_sample_instance_playing(player_kill_sound, false);
                al_set_sample_instance_position(player_kill_sound, 0);
                al_play_sample_instance(player_kill_sound);
                puts("Player died!");
                character->direction = 4;
                al_start_timer(timer);
            }

            if (map_table[y][x] >= 20) {
                al_set_sample_instance_playing(collect_sound, false);
                al_set_sample_instance_position(collect_sound, 0);
                al_play_sample_instance(collect_sound);
                power_up_collect(character, map_table[y][x]);
                map_table[y][x] = 10;
            }

            if (map_table[y][x] >= 11 && map_table[y][x] <= 12) {
                int tile_left = x * TILE_SIZE;
                int tile_right = (x + 1) * TILE_SIZE;
                int tile_top = y * TILE_SIZE;
                int tile_bottom = (y + 1) * TILE_SIZE;

                if (left < tile_right && right > tile_left && top < tile_bottom && bottom > tile_top) {
                    return true;
                }
            }
        }
    }
    return false;
}

/// move_character
/// \brief Funkcja dzięki której gracz może poruszać się po poziomie. Ta funkcja zawiera odwołanie do funkcji check_collision tak aby gracz nie mógł przechodzić przez pudełka i solidne ściany
/// \param character Struktura danych postaci 
/// \param dx Pozycja X gracza
/// \param dy Pozycja Y gracza
/// \param timer Timer na którym działa główna pętla
/// \param player_kill_sound Instacja pliku dźwiękowego w sytuacji, gdy postać umiera
/// \param collect_sound Instacja pliku dźwiękowego w sytuacji, gdy postać podnosi ulepszenie
void move_character(struct Character* character, int dx, int dy, ALLEGRO_TIMER* timer, ALLEGRO_SAMPLE_INSTANCE* player_kill_sound, ALLEGRO_SAMPLE_INSTANCE* collect_sound) {
    character->x += dx;
    character->y += dy;

    if (check_collision(character, dx, dy, timer, player_kill_sound, collect_sound) == true) {
        character->x -= dx;
        character->y -= dy;
    }
}

/// \struct Bomb
/// \brief Funkcjonalność ładunków
typedef struct Bomb {
    bool active; ///< Czy bomba jest aktywna
    bool removed; ///< Czy bomba jest usunięta
    float x, y; ///< Współrzędne bomby
    int bomb_power; ///< Siła wybuchu
    ALLEGRO_TIMER* timer; ///< Licznik detonacji
    ALLEGRO_TIMER* clean_timer; ///< Licznik wyczyszczenia po detonacji
} Bomb;

/// init_bombs
/// \brief Zapełnianie tablicy struktur danymi dla każdego ładunku wszystkich graczy
/// \param Struktura danych bomb
void init_bombs(struct Bomb* bombs) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        bombs[i].active = false;
        bombs[i].removed = true;
        bombs[i].x = 0;
        bombs[i].y = 0;
        bombs[i].bomb_power = 2;
        bombs[i].timer = al_create_timer(1.5);
        bombs[i].clean_timer = al_create_timer(0.4);
    }
}

/// place_bomb
/// \brief Ta funkcja stawia ładunek na pozycji gracza gdy zostanie wciśnięty odpowiedni klawisz
/// \param character Struktura danych gracza
/// \param bombs Struktura danych bomb
/// \param bomb_place_sound Instacja pliku dźwiekowego do stawiania bomb
void place_bomb(struct Character* character, struct Bomb* bombs, ALLEGRO_SAMPLE_INSTANCE* bomb_place_sound)
{
    int x = ((character->x + 24) / TILE_SIZE) * TILE_SIZE;
    int y = ((character->y + 24) / TILE_SIZE) * TILE_SIZE;

    for (int i = 0; i < character->bombs; i++) {

        if (!bombs[i].active && bombs[i].removed) {
            bombs[i].bomb_power = character->power;
            bombs[i].x = x;
            bombs[i].y = y;

            for (int j = 0; j < character->bombs; j++) {
                if (i != j && bombs[j].active) {
                    if (bombs[i].x == bombs[j].x && bombs[i].y == bombs[j].y) {
                        bombs[i].x = 10000;
                        bombs[i].y = 10000;
                        return;
                    }
                }
            }

            bombs[i].active = true;
            bombs[i].removed = false;

            al_set_sample_instance_playing(bomb_place_sound, false);
            al_set_sample_instance_position(bomb_place_sound, 0);
            al_play_sample_instance(bomb_place_sound);
            al_start_timer(bombs[i].timer);

            break;
        }
    }
}

/// draw_bombs
/// \brief Funkcja która wywoływana jest co klatkę. Sprawdza które ładunki są aktywne i w którym miejscu po czym rysuje ładunek na ekranie
/// \param image Bitmapa do rysowania tekstury bomby
/// \param bombs Struktura danych bomb
void draw_bombs(ALLEGRO_BITMAP* image, struct Bomb* bombs) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (bombs[i].active) {
            al_draw_bitmap(image, bombs[i].x, bombs[i].y, 0);
        }
    }
}

/// create_explosion
/// \brief Funkcja do propagacji eksplozji w każdym kierunku oraz niszczenia pudełek [ zoptymalizować ]
/// \param x Pozycja X bomby
/// \param y Pozycja Y bomby
/// \param power Siła wybuchu
/// \param timer Timer głównej pętli gry
/// \param explosion_sound Instancja pliku dźwiękowego w sytuacji, gdy bomba wybucha
void create_explosion(int x, int y, int power, ALLEGRO_TIMER* timer, ALLEGRO_SAMPLE_INSTANCE* explosion_sound) {
    map_table[y][x] = 13;

        for (int i = 0; i < power; i++) {
            if (map_table[y][x + i] == 10 || map_table[y][x + i] >= 20) {
                map_table[y][x + i] = 15;
            }
            if (map_table[y][x + i] == 12) {
                map_table[y][x + i] = 10;
                power_up(x + i, y);
                if (i > 1) {
                    map_table[y][x + i - 1] = 19;
                }
                break;
            }
            if (map_table[y][x + i] == 11) {
                if (i > 1) {
                    map_table[y][x + i - 1] = 19;
                }
                break;
            }
            if (i == power - 1) {
                map_table[y][x + i] = 19;
            }
        }
        for (int i = 0; i < power; i++) {
            if (map_table[y][x - i] == 10 || map_table[y][x - i] >= 20) {
                map_table[y][x - i] = 15;
            }
            if (map_table[y][x - i] == 12) {
                map_table[y][x - i] = 10;
                power_up(x - i, y);
                if (i > 1) {
                    map_table[y][x - i + 1] = 18;
                }
                break;
            }
            if (map_table[y][x - i] == 11) {
                if (i > 1) {
                    map_table[y][x - i + 1] = 18;
                }
                break;
            }
            if (i == power - 1) {
                map_table[y][x - i] = 18;
            }
        }
        for (int i = 0; i < power; i++) {
            if (map_table[y + i][x] == 10 || map_table[y + i][x] >= 20) {
                map_table[y + i][x] = 14;
            }
            if (map_table[y + i][x] == 12) {
                map_table[y + i][x] = 10;
                power_up(x, y + i);
                if (i > 1) {
                    map_table[y + i - 1][x] = 17;
                }
                break;
            }
            if (map_table[y + i][x] == 11) {
                if (i > 1) {
                    map_table[y + i - 1][x] = 17;
                }
                break;
            }
            if (i == power - 1) {
                map_table[y + i][x] = 17;
            }
        }
        for (int i = 0; i < power; i++) {
            if (map_table[y - i][x] == 10 || map_table[y - i][x] >= 20) {
                map_table[y - i][x] = 14;
            }
            if (map_table[y - i][x] == 12) {
                map_table[y - i][x] = 10;
                power_up(x, y - i);
                if (i > 1) {
                    map_table[y - i + 1][x] = 16;
                }
                break;
            }
            if (map_table[y - i][x] == 11) {
                if (i > 1) {
                    map_table[y - i + 1][x] = 16;
                }
                break;
            }
            if (i == power - 1) {
                map_table[y - i][x] = 16;
            }
        }

    al_set_sample_instance_playing(explosion_sound, false);
    al_set_sample_instance_position(explosion_sound, 0);
    al_play_sample_instance(explosion_sound);
    al_start_timer(timer);
}

/// remove_explosion
/// /brief Funkcja która usuwa pola eksplozji po wybuchu ładunku z opóźnieniem
/// \param x Pozycja X bomby
/// \param y pozycja Y bomby
/// \param power Siła wybuchu
/// \param timer Timer głównej pętli gry
void remove_explosion(int x, int y, int power, ALLEGRO_TIMER* timer) {
    map_table[y][x] = 10;

    for (int i = 0; i < power; i++) {
        if (map_table[y][x + i] >= 13 && map_table[y][x + i] < 20) {
            map_table[y][x + i] = 10;
        }
    }
    for (int i = 0; i < power; i++) {
        if (map_table[y][x - i] >= 13 && map_table[y][x - i] < 20) {
            map_table[y][x - i] = 10;
        }
    }
    for (int i = 0; i < power; i++) {
        if (map_table[y + i][x] >= 13 && map_table[y + i][x] < 20) {
            map_table[y + i][x] = 10;
        }
    }
    for (int i = 0; i < power; i++) {
        if (map_table[y - i][x] >= 13 && map_table[y - i][x] < 20) {
            map_table[y - i][x] = 10;
        }
    }

    al_stop_timer(timer);
    al_set_timer_count(timer, 0);
}

/// update_bombs
/// \brief Funkcja która działą co klatkę, sprawdza na bierząco stan każdego ładunku. Gdy timer któregoś z ładunków się zakończy to wywołuje funkcje która tworzy eksplozję.
/// \param character Struktura danych gracza 1
/// \param character2 Struktura danych gracza 2
/// \param bombs Struktura danych bomb
/// \param timer Timer głównej pętli gry
/// \param explosion_sound Instancja pliku dźwiękowego w sytuacji, gdy wybucha bomba
/// \param player_kill_sound Instacja pliku dźwiękowego w sytuacji, gdy gracz umiera
/// \param collect_sound Instacja pliku dźwiękowego w sytuacji, gdy gracz podnosi ulepszenie
void update_bombs(struct Character* character, struct Character* character2, struct Bomb* bombs, ALLEGRO_TIMER* timer, ALLEGRO_SAMPLE_INSTANCE* explosion_sound, ALLEGRO_SAMPLE_INSTANCE* player_kill_sound, ALLEGRO_SAMPLE_INSTANCE* collect_sound) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (al_get_timer_count(bombs[i].clean_timer) >= al_get_timer_speed(bombs[i].clean_timer)) {
            remove_explosion(bombs[i].x / TILE_SIZE, bombs[i].y / TILE_SIZE, bombs[i].bomb_power, bombs[i].clean_timer);
            bombs[i].removed = true;
            bombs[i].x = 10000;
            bombs[i].y = 10000;
        }
        if (bombs[i].active) {
            if (al_get_timer_count(bombs[i].timer) >= al_get_timer_speed(bombs[i].timer)) {
                create_explosion(bombs[i].x / TILE_SIZE, bombs[i].y / TILE_SIZE, bombs[i].bomb_power, bombs[i].clean_timer, explosion_sound);
                bombs[i].active = false;

                check_collision(character, 0, 0, timer, player_kill_sound, collect_sound);
                check_collision(character2, 0, 0, timer, player_kill_sound, collect_sound);

                al_stop_timer(bombs[i].timer);
                al_set_timer_count(bombs[i].timer, 0);
            }
        }
    }
}

///Główna funkcja
int main() {

    ///Inicjalizowanie ekranu oraz kolejki zdarzeń
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* queue = NULL;

    ///Inicjalizowanie tekstur graficznych
    ALLEGRO_BITMAP* brick = NULL;
    ALLEGRO_BITMAP* box = NULL;
    ALLEGRO_BITMAP* explosion_center = NULL;
    ALLEGRO_BITMAP* explosion_vertical = NULL;
    ALLEGRO_BITMAP* explosion_horizontal = NULL;
    ALLEGRO_BITMAP* tip_top = NULL;
    ALLEGRO_BITMAP* tip_bottom = NULL;
    ALLEGRO_BITMAP* tip_left = NULL;
    ALLEGRO_BITMAP* tip_right = NULL;
    ALLEGRO_BITMAP* fire_up = NULL;
    ALLEGRO_BITMAP* bomb_up = NULL;
    ALLEGRO_BITMAP* speed_up = NULL;
    ALLEGRO_BITMAP* bomb = NULL;

    ///Inicjalizowanie tekstur podglądu mapy
    ALLEGRO_BITMAP* level_thumbnail_0 = NULL;
    ALLEGRO_BITMAP* level_thumbnail_1 = NULL;
    ALLEGRO_BITMAP* level_thumbnail_2 = NULL;
    ALLEGRO_BITMAP* level_thumbnail_3 = NULL;
    ALLEGRO_BITMAP* level_thumbnail_4 = NULL;

    ///Inicjalizowanie tekstur graczy
    ALLEGRO_BITMAP* player1_dead = NULL;
    ALLEGRO_BITMAP* player2_dead = NULL;
    ALLEGRO_BITMAP* player1_spritesheet = NULL;
    ALLEGRO_BITMAP* player2_spritesheet = NULL;

    //Swiatlo
    ALLEGRO_BITMAP* light_shape_gradient = NULL;
    ALLEGRO_BITMAP* light_shape_cross = NULL;
    ALLEGRO_BITMAP* light_map = NULL;

    ///Inicjalizowanie obsługi animacji
    ALLEGRO_TIMER* animation1_timer = NULL;
    ALLEGRO_TIMER* animation2_timer = NULL;
    ALLEGRO_TIMER* reset_timer = NULL;
    ALLEGRO_TIMER* framerate_timer = NULL;

    ///Inicjalizowanie obsługi klawiatury
    ALLEGRO_KEYBOARD_STATE keyState;

    ///Inicjalizowanie czcionek
    ALLEGRO_FONT* font = NULL;
    ALLEGRO_FONT* font_lower = NULL;

    ///Inicjalizowanie obsługi dźwięków
    ALLEGRO_SAMPLE* explosion_sample = NULL;
    ALLEGRO_SAMPLE* collect_sample = NULL;
    ALLEGRO_SAMPLE* player_kill_sample = NULL;
    ALLEGRO_SAMPLE* bomb_place_sample = NULL;
    ALLEGRO_SAMPLE* soundtrack_sample = NULL;

    ALLEGRO_SAMPLE_INSTANCE* explosion_instance = NULL;
    ALLEGRO_SAMPLE_INSTANCE* collect_instance = NULL;
    ALLEGRO_SAMPLE_INSTANCE* player_kill_instance = NULL;
    ALLEGRO_SAMPLE_INSTANCE* bomb_place_instance = NULL;
    ALLEGRO_SAMPLE_INSTANCE* soundtrack_instance = NULL;

    ///Wyowałnie inicjalizacji potrzebnych rozszerzeń allegro
    al_init();
    al_install_audio();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_acodec_addon();
    al_init_primitives_addon();

    ///Deklaracja bomb graczy
    Bomb bombs1[MAX_BOMBS];
    Bomb bombs2[MAX_BOMBS];

    ///Wyowałnie inicjalizacji bomb
    init_bombs(bombs1);
    init_bombs(bombs2);

    ///Przypisanie domyślnych wartości do główynch składowych programu
    display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    queue = al_create_event_queue();
    framerate_timer = al_create_timer(1.0 / FPS);
    reset_timer = al_create_timer(2);
    animation1_timer = al_create_timer(1.0 / 12.0);
    animation2_timer = al_create_timer(1.0 / 12.0);

    ///Inicjalizacja obsługi klawiatury oraz myszki
    al_install_keyboard();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_hide_mouse_cursor(display);

    ///Zarejestrowanie zdarzeń związanych z timerami
    al_register_event_source(queue, al_get_timer_event_source(framerate_timer));
    al_register_event_source(queue, al_get_timer_event_source(reset_timer));
    al_register_event_source(queue, al_get_timer_event_source(animation1_timer));
    al_register_event_source(queue, al_get_timer_event_source(animation2_timer));

    ///Rozpoczęcie działania timerów
    al_start_timer(framerate_timer);
    al_start_timer(animation1_timer);
    al_start_timer(animation2_timer);

    ///Przypisanie odpowiednich tekstur do graficznej reprezentacji poziomu
    brick = al_load_bitmap("brick.png");
    box = al_load_bitmap("box.png");
    explosion_center = al_load_bitmap("explosion_center.png");
    explosion_vertical = al_load_bitmap("explosion_vertical.png");
    explosion_horizontal = al_load_bitmap("explosion_horizontal.png");
    tip_top = al_load_bitmap("tip_top.png");
    tip_bottom = al_load_bitmap("tip_bottom.png");
    tip_left = al_load_bitmap("tip_left.png");
    tip_right = al_load_bitmap("tip_right.png");
    fire_up = al_load_bitmap("fire_up.png");
    bomb_up = al_load_bitmap("bomb_up.png");
    speed_up = al_load_bitmap("speed_up.png");
    bomb = al_load_bitmap("bomb.png");

    ///Przypisanie odpowiednich tekstur do graficznej reprezentacji graczy
    player1_dead = al_load_bitmap("player1_dead.png");
    player2_dead = al_load_bitmap("player2_dead.png");
    player1_spritesheet = al_load_bitmap("player1_spritesheet.png");
    player2_spritesheet = al_load_bitmap("player2_spritesheet.png");

    ///Przypisanie odpowiednich tekstur do graficznej reprezentacji podglądu map przy wyborze z menu
    level_thumbnail_0 = al_load_bitmap("poziom0.png");
    level_thumbnail_1 = al_load_bitmap("poziom1.png");
    level_thumbnail_2 = al_load_bitmap("poziom2.png");
    level_thumbnail_3 = al_load_bitmap("poziom3.png");
    level_thumbnail_4 = al_load_bitmap("poziom4.png");

    //Przypisanie bitmap do swiatla
    light_shape_gradient = al_load_bitmap("light_shape_gradient.png");
    light_shape_cross = al_load_bitmap("light_shape_cross.png");
    light_map = al_create_bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    ///Przypisanie pliku "true type font" do obsługi czcionki
    font = al_load_font("slkscreb.ttf", 40, 0);
    font_lower = al_load_font("slkscreb.ttf", 30, 0);

    ///Przypisanie plików dźwiękowych do obsługi oprawy dźwięku
    explosion_sample = al_load_sample("explosion.mp3");
    collect_sample = al_load_sample("collect.mp3");
    player_kill_sample = al_load_sample("player_kill.mp3");
    bomb_place_sample = al_load_sample("bomb_place.mp3");
    soundtrack_sample = al_load_sample("soundtrack.mp3");

    ///Stworzenie instacji dźwiękowych do odtwarzania
    explosion_instance = al_create_sample_instance(explosion_sample);
    collect_instance = al_create_sample_instance(collect_sample);
    player_kill_instance = al_create_sample_instance(player_kill_sample);
    bomb_place_instance = al_create_sample_instance(bomb_place_sample);
    soundtrack_instance = al_create_sample_instance(soundtrack_sample);

    ///Asercja poprawnego załadowania tekstur
    assert(brick != NULL);
    assert(box != NULL);
    assert(explosion_center != NULL);
    assert(explosion_vertical != NULL);
    assert(explosion_horizontal != NULL);
    assert(tip_top != NULL);
    assert(tip_bottom != NULL);
    assert(tip_left != NULL);
    assert(tip_right != NULL);
    assert(fire_up != NULL);
    assert(bomb_up != NULL);
    assert(speed_up != NULL);
    assert(bomb != NULL);
    assert(player2_dead != NULL);
    assert(player1_dead != NULL);
    assert(player1_spritesheet != NULL);
    assert(player2_spritesheet != NULL);
    assert(light_shape_gradient != NULL);
    assert(light_map != NULL);

    ///Zmienna twierdząca czy program ma być wykonywany
    bool running = true;

    ///Zmienne domyślnych pozycji graczy
    float default_x1 = TILE_SIZE;
    float default_y1 = TILE_SIZE;
    float default_x2 = default_x1 * MAP_WIDTH - 2 * TILE_SIZE;
    float default_y2 = default_y1 * MAP_HEIGHT - 2 * TILE_SIZE;

    ///Zmienne domyślnych wartości ulepszeń
    int default_speed = 5;
    int default_power = 2;
    int default_bombs = 1;

    ///Zmienne używane do animacji graczy
    int animation1_frame = 0;
    int animation2_frame = 0;
    bool running1 = false;
    bool running2 = false;

    ///Zmienne do obsługi graficznego menu głównego
    bool menu_Option = 1;
    bool pause_Option = 0;
    bool level_Option = 0;

    ///Zmienne do obsługi graficznego menu pauzy
    short optionSelPause = 0;
    short optionSelMenu = 0;
    short optionSelLevel = 0;
    bool choose_guard = false
        ;
    ///Inicjalizacja graczy
    struct Character plr1 = create_character(player1_dead, player1_spritesheet, default_x1, default_y1, default_speed, default_bombs, default_power);
    struct Character plr2 = create_character(player2_dead, player2_spritesheet, default_x2, default_y2, default_speed, default_bombs, default_power);

    ///Rezerwacja ilości używanych dźwięków miksera oraz ich domyślne wartości
    al_reserve_samples(6);
    al_set_sample_instance_playmode(soundtrack_instance, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(soundtrack_instance, 0.11);
    al_attach_sample_instance_to_mixer(soundtrack_instance, al_get_default_mixer());

    al_set_sample_instance_playmode(explosion_instance, ALLEGRO_PLAYMODE_ONCE);
    al_set_sample_instance_gain(explosion_instance, 2.0);
    al_attach_sample_instance_to_mixer(explosion_instance, al_get_default_mixer());

    al_set_sample_instance_playmode(collect_instance, ALLEGRO_PLAYMODE_ONCE);
    al_set_sample_instance_gain(collect_instance, 1.0);
    al_attach_sample_instance_to_mixer(collect_instance, al_get_default_mixer());

    al_set_sample_instance_playmode(player_kill_instance, ALLEGRO_PLAYMODE_ONCE);
    al_set_sample_instance_gain(player_kill_instance, 1.0);
    al_attach_sample_instance_to_mixer(player_kill_instance, al_get_default_mixer());

    al_set_sample_instance_playmode(bomb_place_instance, ALLEGRO_PLAYMODE_ONCE);
    al_set_sample_instance_gain(bomb_place_instance, 1.0);
    al_attach_sample_instance_to_mixer(bomb_place_instance, al_get_default_mixer());

    ///Rozpoczęcie odtwarzania dźwięku w tle
    al_play_sample_instance(soundtrack_instance);

    ///Pętla główna gry
    while (running) {
        ///Zarejestrowanie obsłubi klawiatury i eventów
        al_get_keyboard_state(&keyState);
        ALLEGRO_EVENT event;
        al_clear_to_color(al_map_rgba_f(0, 255, 0, 0));

        ///Asercja o pustej kolejce zdarzeń
        if (!al_is_event_queue_empty(queue)) {
            al_wait_for_event(queue, &event);

            ///Wyowałnie graficzne menu głównego
            if (menu_Option) {
                
                ///Reprezentacja graficzna opcji do wyboru
                al_clear_to_color(al_map_rgba(20, 20, 20, 0.9));
                al_draw_text(font, al_map_rgb(255, 0, 0), DISPLAY_WIDTH / 2, 10, ALLEGRO_ALIGN_CENTER, "MAIN MENU");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 20, 100, ALLEGRO_ALIGN_LEFT, "PLAY");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 20, 150, ALLEGRO_ALIGN_LEFT, "EXIT");

                ///Obsługa zdarzeń klawiatury
                if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (al_key_down(&keyState, ALLEGRO_KEY_DOWN) || al_key_down(&keyState, ALLEGRO_KEY_S)) {
                        if (optionSelMenu < 1) {
                            optionSelMenu++;
                        }
                        else {
                            optionSelMenu = 0;
                        }
                    }
                    if (al_key_down(&keyState, ALLEGRO_KEY_UP) || al_key_down(&keyState, ALLEGRO_KEY_W)) {
                        if (optionSelMenu > 0) {
                            optionSelMenu--;
                        }
                        else {
                            optionSelMenu = 1;
                        }
                    }
                }

                ///Obsługa zdarzenia zamknięcia okna
                if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = false;
                    menu_Option = false;
                }

                ///Obsługa reprezentacji graficznej oraz zmiany stanu menu głównego
                switch (optionSelMenu) {
                case 0: al_draw_text(font_lower, al_map_rgb(255, 0, 0), 200, 100, 0, "<-");
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {

                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER)) {
                            menu_Option = false;
                            level_Option = true;
                        }
                    }
                    break;
                case 1: al_draw_text(font_lower, al_map_rgb(255, 0, 0), 200, 150, 0, "<-"); 
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN) { 
                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER)) { 
                            running = false; menu_Option = false; 
                        } 
                    } 
                    break;
                }
                
                ///Odświerzenie ekranu oraz przeskoczenie do kolejnego cyklu pętli
                al_flip_display();
                continue;
            }

            ///Wywołanie graficznego menu wyboru poziomu
            if (level_Option)
            {

                ///Reprezentacja graficzna opcji do wyboru
                al_clear_to_color(al_map_rgba(20, 20, 20,0.9));
                al_draw_text(font, al_map_rgb(255, 0, 0), DISPLAY_WIDTH/2, 10, ALLEGRO_ALIGN_CENTER, "SELECT LEVEL");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 20, 100, ALLEGRO_ALIGN_LEFT, "CLASSIC");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 20, 150, ALLEGRO_ALIGN_LEFT, "FANCY");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 20, 200, ALLEGRO_ALIGN_LEFT, "MAZE");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 20, 250, ALLEGRO_ALIGN_LEFT, "SNAKE");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 20, 300, ALLEGRO_ALIGN_LEFT, "BOX");
                al_draw_rectangle(DISPLAY_WIDTH-400,100,DISPLAY_WIDTH-100,400,al_map_rgb(255,0,0),1);

                ///Obsługa zdarzenia zamknięcia okna
                if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                {
                    running = false;
                    level_Option = false;
                    menu_Option = false;
                }

                ///Obsługa zdarzeń klawiatury
                if (event.type == ALLEGRO_EVENT_KEY_DOWN)
                {
                    if (al_key_down(&keyState, ALLEGRO_KEY_DOWN) || al_key_down(&keyState, ALLEGRO_KEY_S)) {
                        if (optionSelLevel < 4) {
                            optionSelLevel++;
                        }
                        else {
                            optionSelLevel = 0;
                        }
                    }
                    if (al_key_down(&keyState, ALLEGRO_KEY_UP) || al_key_down(&keyState, ALLEGRO_KEY_W)) {
                        if (optionSelLevel > 0) {
                            optionSelLevel--;
                        }
                        else {
                            optionSelLevel = 4;
                        }
                    }
                    if (al_key_down(&keyState, ALLEGRO_KEY_ESCAPE))
                    {
                        menu_Option = true;
                        level_Option = false;
                        al_flip_display();
                        continue;
                    }
                }

                ///Obsługa zdarzeń wybranych opcji
                switch (optionSelLevel)
                {
                case 0: 
                    al_draw_text(font_lower, al_map_rgb(255, 0, 0), 230, 100, 0, "<-");
                    al_draw_bitmap(level_thumbnail_0, DISPLAY_WIDTH - 390, 110, 0);
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN)
                    {
                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER))
                        {
                            al_play_sample_instance(explosion_instance);
                            choose_guard = true;
                        }
                    }
                    break;
                case 1:
                    al_draw_text(font_lower, al_map_rgb(255, 0, 0), 230, 150, 0, "<-");
                    al_draw_bitmap(level_thumbnail_1, DISPLAY_WIDTH - 390, 110, 0);
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN)
                    {
                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER))
                        { 
                            al_play_sample_instance(explosion_instance);
                            choose_guard = true;
                        }
                    }
                    break;
                case 2:
                    al_draw_text(font_lower, al_map_rgb(255, 0, 0), 230, 200, 0, "<-");
                    al_draw_bitmap(level_thumbnail_2, DISPLAY_WIDTH - 390, 110, 0);
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN)
                    {
                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER))
                        {   
                            al_play_sample_instance(explosion_instance);
                            choose_guard = true;
                        }
                    }
                    break;
                case 3:
                    al_draw_text(font_lower, al_map_rgb(255, 0, 0), 230, 250, 0, "<-");
                    al_draw_bitmap(level_thumbnail_3, DISPLAY_WIDTH - 390, 110, 0);
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN)
                    {
                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER))
                        {
                            al_play_sample_instance(explosion_instance);
                            choose_guard = true;
                        }
                    }
                    break;
                case 4:
                    al_draw_text(font_lower, al_map_rgb(255, 0, 0), 230, 300, 0, "<-");
                    al_draw_bitmap(level_thumbnail_4, DISPLAY_WIDTH - 390, 110, 0);
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN)
                    {
                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER))
                        {
                            al_play_sample_instance(explosion_instance);
                            choose_guard = true;
                        }
                    }
                    break;
                }

                ///Obsługa zdarzenia po wybraniu poziomu
                ///Załadowanie poziomu do tablicy
                ///Zresetowanie graczy do domyślnych wartości
                ///Inicjalizacja bomb
                if (choose_guard)
                {
                    load_map_table(optionSelLevel);
                    level_Option = false;
                    menu_Option = false;
                    puts("Reset.");

                    int i, j;
                    for (i = 0; i < MAP_WIDTH; i++) {
                        for (j = 0; j < MAP_HEIGHT; j++) {
                            map_table[j][i] = map_template[j][i];
                        }
                    }

                    plr1 = create_character(player1_dead, player1_spritesheet, default_x1, default_y1, default_speed, default_bombs, default_power);
                    plr2 = create_character(player2_dead, player2_spritesheet, default_x2, default_y2, default_speed, default_bombs, default_power);

                    init_bombs(bombs1);
                    init_bombs(bombs2);

                    al_stop_timer(reset_timer);
                    al_set_timer_count(reset_timer, 0);
                    printf("\n\n%d\n", optionSelLevel);
                    choose_guard = false;
                }

                ///Odświerzenie ekranu oraz przeskoczenie cyklu pętli
                al_flip_display();
                continue;
            }

            ///Reprezentacja graficzna menu pauzy
            if (pause_Option) {

                ///Zatrzymanie odliczania wszystkich bomb
                for (int i = 0; i < MAX_BOMBS; i++) {
                    al_stop_timer(bombs1[i].timer);
                    al_stop_timer(bombs2[i].timer);
                }

                ///Reprezentacja graficzna opcji do wyboru
                al_clear_to_color(al_map_rgba(20, 20, 20, 0.9));
                al_draw_text(font, al_map_rgb(255, 0, 0), MAP_WIDTH * TILE_SIZE / 2, 10, ALLEGRO_ALIGN_CENTER, "PAUSE");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 10, 50, ALLEGRO_ALIGN_LEFT, "Continue");
                al_draw_text(font_lower, al_map_rgb(255, 0, 0), 10, 100, ALLEGRO_ALIGN_LEFT, "Main Menu");

                ///Obsługa zdarzeń klawiatury
                if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (al_key_down(&keyState, ALLEGRO_KEY_DOWN) || al_key_down(&keyState, ALLEGRO_KEY_S)) {
                        if (optionSelPause < 1) {
                            optionSelPause++;
                        }
                        else {
                            optionSelPause = 0;
                        }
                    }
                    if (al_key_down(&keyState, ALLEGRO_KEY_UP) || al_key_down(&keyState, ALLEGRO_KEY_W)) {
                        if (optionSelPause > 0) {
                            optionSelPause--;
                        }
                        else {
                            optionSelPause = 1;
                        }
                    }
                    if (al_key_down(&keyState, ALLEGRO_KEY_ESCAPE)) {
                        pause_Option = false;
                    }
                }

                ///Obsługa zdarzeń wybranej opcji
                switch (optionSelPause) {
                case 0:
                    al_draw_text(font_lower, al_map_rgb(255, 0, 0), 300, 50, 0, "<-");
                    if (al_key_down(&keyState, ALLEGRO_KEY_ENTER)) {
                        pause_Option = false;
                    }
                    for (int i = 0; i < MAX_BOMBS; i++) {
                        al_start_timer(bombs1[i].timer);
                        al_start_timer(bombs2[i].timer);
                    }
                    break;
                case 1:
                    al_draw_text(font_lower, al_map_rgb(255, 0, 0), 300, 100, 0, "<-");
                    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                        if (al_key_down(&keyState, ALLEGRO_KEY_ENTER)) {
                            pause_Option = false; menu_Option = true;
                            break;
                        }
                    }
                }
                
                ///Obsługa zdarzenia zamknięcia okna
                if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = false;
                    pause_Option = false;
                }

                ///Odświerzenie ekranu oraz przeskoczenie cyklu pętli
                al_flip_display();
                continue;
            }
            
            ///Obsługa zdarzenia klawiatury po wciśnięciu ESC, zatrzymanie gry i wywołanie menu pauzy
            if (al_key_down(&keyState, ALLEGRO_KEY_ESCAPE)) {
                pause_Option = true;
                if (optionSelMenu != 0) {
                    optionSelMenu = 0;
                }
                if (optionSelPause != 0) {
                    optionSelPause = 0;
                }
            }


            ///Obsługa zdarzenia zamknięcia okna
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;

            ///Zaktualizowanie danych bomb dla obu graczy
            update_bombs(&plr1, &plr2, bombs1, reset_timer, explosion_instance, player_kill_instance, collect_instance);
            update_bombs(&plr2, &plr1, bombs2, reset_timer, explosion_instance, player_kill_instance, collect_instance);

            ///Obsługa zdarzeń klawiatury w celu stawiania bomb
            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    place_bomb(&plr1, bombs1, bomb_place_instance);
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    place_bomb(&plr2, bombs2, bomb_place_instance);
                }
            }

            ///Obsługa zdarzeń timerów wykorzystywanych do animacji postaci
            if (event.type == ALLEGRO_EVENT_TIMER) {

                ///postać gracza 1
                if (event.timer.source == animation1_timer && running1) {

                    if (animation1_frame < 4) {
                        animation1_frame++;
                    }
                    else {
                        animation1_frame = 1;
                    }

                    running1 = false;
                }

                ///postać gracza 2
                if (event.timer.source == animation2_timer && running2) {

                    if (animation2_frame < 4) {
                        animation2_frame++;
                    }
                    else {
                        animation2_frame = 1;
                    }

                    running2 = false;
                }

                ///Restartowanie poziomu gdy jeden z graczy zostanie złapany w wybuchu
                if (event.timer.source == reset_timer) {
                    puts("Reset.");

                    int i, j;
                    for (i = 0; i < MAP_WIDTH; i++) {
                        for (j = 0; j < MAP_HEIGHT; j++) {
                            map_table[j][i] = map_template[j][i];
                        }
                    }

                    plr1 = create_character(player1_dead, player1_spritesheet, default_x1, default_y1, default_speed, default_bombs, default_power);
                    plr2 = create_character(player2_dead, player2_spritesheet, default_x2, default_y2, default_speed, default_bombs, default_power);

                    init_bombs(bombs1);
                    init_bombs(bombs2);

                    al_stop_timer(reset_timer);
                    al_set_timer_count(reset_timer, 0);
                }

                ///Timer który ogranicza akcje do 60 klatek na sekundę
                if (event.timer.source == framerate_timer) {

                    ///Gracz 1
                    if (plr1.direction != 4) {
                        ///Kierunek góra
                        if (al_key_down(&keyState, ALLEGRO_KEY_W)) {
                            plr1.direction = 0;
                            running1 = true;
                            move_character(&plr1, 0, -plr1.speed, reset_timer, player_kill_instance, collect_instance);
                        }
                        ///Kierunek dół
                        if (al_key_down(&keyState, ALLEGRO_KEY_S)) {
                            plr1.direction = 1;
                            running1 = true;
                            move_character(&plr1, 0, plr1.speed, reset_timer, player_kill_instance, collect_instance);
                        }
                        ///Kierunek lewo
                        if (al_key_down(&keyState, ALLEGRO_KEY_A)) {
                            plr1.direction = 2;
                            running1 = true;
                            move_character(&plr1, -plr1.speed, 0, reset_timer, player_kill_instance, collect_instance);
                        }
                        ///Kierunek prawo
                        if (al_key_down(&keyState, ALLEGRO_KEY_D)) {
                            plr1.direction = 3;
                            running1 = true;
                            move_character(&plr1, plr1.speed, 0, reset_timer, player_kill_instance, collect_instance);
                        }
                    }

                    if (!running1) {
                        animation1_frame = 0;
                    }

                    ///Gracz 2
                    if (plr2.direction != 4) {
                        ///Kierunek góra
                        if (al_key_down(&keyState, ALLEGRO_KEY_UP)) {
                            plr2.direction = 0;
                            running2 = true;
                            move_character(&plr2, 0, -plr2.speed, reset_timer, player_kill_instance, collect_instance);
                        }
                        ///Kierunek dół
                        if (al_key_down(&keyState, ALLEGRO_KEY_DOWN)) {
                            plr2.direction = 1;
                            running2 = true;
                            move_character(&plr2, 0, plr2.speed, reset_timer, player_kill_instance, collect_instance);
                        }
                        ///Kierunek lewo
                        if (al_key_down(&keyState, ALLEGRO_KEY_LEFT)) {
                            plr2.direction = 2;
                            running2 = true;
                            move_character(&plr2, -plr2.speed, 0, reset_timer, player_kill_instance, collect_instance);
                        }
                        ///Kierunek prawo
                        if (al_key_down(&keyState, ALLEGRO_KEY_RIGHT)) {
                            plr2.direction = 3;
                            running2 = true;
                            move_character(&plr2, plr2.speed, 0, reset_timer, player_kill_instance, collect_instance);
                        }
                    }

                    if (!running2) {
                        animation2_frame = 0;
                    }

                    draw_bombs(bomb, bombs1);
                    draw_bombs(bomb, bombs2);

                    if (!menu_Option && !level_Option && !pause_Option)
                    {
                        al_set_target_bitmap(light_map);
                        al_clear_to_color(al_map_rgba(0, 0, 0, 255));
                        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
                    }

                    al_draw_tinted_bitmap(light_shape_gradient, al_map_rgba_f(1, 0, 0, 0.5), plr1.x - 226, plr1.y - 236, 0);
                    al_draw_tinted_bitmap(light_shape_gradient, al_map_rgba_f(0, 0, 1, 0.5), plr2.x - 226, plr2.y - 236, 0);

                    al_set_target_bitmap(al_get_backbuffer(display));

                    ///Obsługa arkusza animacji gracza 1
                    if (plr1.direction == 0) {
                        al_draw_bitmap_region(plr1.spritesheet, plr1.direction * PLAYER_SIZE, animation1_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr1.x, plr1.y, 0);
                    }
                    else if (plr1.direction == 1) {
                        al_draw_bitmap_region(plr1.spritesheet, plr1.direction * PLAYER_SIZE, animation1_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr1.x, plr1.y, 0);
                    }
                    else if (plr1.direction == 2) {
                        al_draw_bitmap_region(plr1.spritesheet, plr1.direction * PLAYER_SIZE, animation1_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr1.x, plr1.y, 0);
                    }
                    else if (plr1.direction == 3) {
                        al_draw_bitmap_region(plr1.spritesheet, plr1.direction * PLAYER_SIZE, animation1_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr1.x, plr1.y, 0);
                    }
                    else if (plr1.direction == 4) {
                        al_draw_bitmap(plr1.image_dead, plr1.x, plr1.y, 0);
                    }

                    ///Obsługa arkusza animacji gracza 2
                    if (plr2.direction == 0) {
                        al_draw_bitmap_region(plr2.spritesheet, plr2.direction * PLAYER_SIZE, animation2_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr2.x, plr2.y, 0);
                    }
                    else if (plr2.direction == 1) {
                        al_draw_bitmap_region(plr2.spritesheet, plr2.direction * PLAYER_SIZE, animation2_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr2.x, plr2.y, 0);
                    }
                    else if (plr2.direction == 2) {
                        al_draw_bitmap_region(plr2.spritesheet, plr2.direction * PLAYER_SIZE, animation2_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr2.x, plr2.y, 0);
                    }
                    else if (plr2.direction == 3) {
                        al_draw_bitmap_region(plr2.spritesheet, plr2.direction * PLAYER_SIZE, animation2_frame * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, plr2.x, plr2.y, 0);
                    }
                    else if (plr2.direction == 4) {
                        al_draw_bitmap(plr2.image_dead, plr2.x, plr2.y, 0);
                    }
                    ///Wyrysowanie graficzne mapy
                    load_map(map_table, brick, box,
                        explosion_center, explosion_vertical, explosion_horizontal,
                        tip_top, tip_bottom, tip_left, tip_right,
                        fire_up, bomb_up, speed_up,
                        light_shape_cross, light_map, display);


                    ///Rysowanie bomb

                    al_draw_bitmap(light_map, 0, 0, 0);                    
                    //al_draw_bitmap(light_shape_gradient, plr1.x - 100, plr1.y - 100, 0);
                    //al_draw_bitmap(light_shape_gradient, plr2.x - 100, plr2.y - 100, 0);
                    //printf("\nplayer1: %d %d\nplayer2: %d %d\n", plr1.y, plr1.x, plr2.y, plr1.x);
                    al_flip_display();
                }
            }
        }
    }

    ///Czyszczenie wszystkich elementów
    al_destroy_display(display);

    al_destroy_bitmap(brick);
    al_destroy_bitmap(box);
    al_destroy_bitmap(explosion_center);
    al_destroy_bitmap(explosion_vertical);
    al_destroy_bitmap(explosion_horizontal);
    al_destroy_bitmap(tip_top);
    al_destroy_bitmap(tip_bottom);
    al_destroy_bitmap(tip_left);
    al_destroy_bitmap(tip_right);
    al_destroy_bitmap(fire_up);
    al_destroy_bitmap(bomb_up);
    al_destroy_bitmap(speed_up);
    al_destroy_bitmap(bomb);

    al_destroy_bitmap(player1_dead);
    al_destroy_bitmap(player2_dead);
    al_destroy_bitmap(player1_spritesheet);
    al_destroy_bitmap(player2_spritesheet);

    al_destroy_bitmap(light_shape_gradient);
    al_destroy_bitmap(light_map);

    al_destroy_timer(animation1_timer);
    al_destroy_timer(animation2_timer);
    al_destroy_timer(reset_timer);
    al_destroy_timer(framerate_timer);

    al_destroy_font(font);
    al_destroy_font(font_lower);

    al_destroy_sample(explosion_sample);
    al_destroy_sample(collect_sample);
    al_destroy_sample(player_kill_sample);
    al_destroy_sample(bomb_place_sample);
    al_destroy_sample(soundtrack_sample);

    al_destroy_sample_instance(explosion_instance);
    al_destroy_sample_instance(collect_instance);
    al_destroy_sample_instance(player_kill_instance);
    al_destroy_sample_instance(bomb_place_instance);
    al_destroy_sample_instance(soundtrack_instance);

    al_uninstall_keyboard();
    al_uninstall_mouse();

    return 0;
}
