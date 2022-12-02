#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shader.cpp"
#include "texture.cpp"
#include "resource_manager.cpp"
#include "sprite_renderer.cpp"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_LOSE
}; 

const unsigned int window_width = 800;
const unsigned int window_height = 800;

class Object{
    public:

    glm::vec2 position;
    float size;
    glm::vec2 velocity;
    glm::vec2 center_coords;
    


    Object(){
        position = glm::vec2(0.0f, 0.0f);
        velocity = glm::vec2(0.0f, 0.0f);
    }  

    Object(float x, float y){
        position = glm::vec2(x, y);

    }  

    bool touchingLeftWall(){// returns true if player is touching any wall
        return position.x <= 0;
    }
    bool touchingRightWall(){
        return (position.x >= static_cast<float>(window_width)-size);
    }
    bool touchingUpperWall(){
        return position.y <= 0;
    }
    bool touchingLowerWall(){
        return (position.y >= static_cast<float>(window_height)-size);
    }
    
    bool isTouching(Object obj){

        return center_coords.x >= obj.position.x && center_coords.x <= obj.position.x+size && center_coords.y >= obj.position.y && center_coords.y <= obj.position.y+size;
    }
    void calculate_center_coords(){
        center_coords = glm::vec2(position.x+size/2,position.y+size/2);
        return;
    }


};

class Player: public Object{ //player object
    public:
    float moveSpeed;
    Player(){
        position = glm::vec2(static_cast<float>(window_width)/2,static_cast<float>(window_height)/2);
        moveSpeed = 300;//player moves this fast
        size = 75;
    }

    

};

class Bullet: public Object{ //bullet object
    public:

    bool canBounce = 0;

    // Bullet(){

    // }

    Bullet(float x = 0, float y = 0, float vx = 0, float vy = 0, bool bounce = 0, float s = 100){
        position = glm::vec2(x, y);
        velocity = glm::vec2(vx, vy);
        canBounce = bounce;
        size = s;
    }  

};

// glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

SpriteRenderer *Renderer;

class Game{
    public:
        // game state
        GameState State;

        Player player;

        std::vector<Bullet*> Bullets; //vector of bullets that are active

        bool Keys[1024]; //array of bools for when keys are pressed

        unsigned int Width, Height;//screen dimensions

        // constructor/destructor
        Game(unsigned int width, unsigned int height): Width(width), Height(height){}

        ~Game(){}

        // initialize game state (load all shaders/textures/levels)
        void Init(){

            State = GAME_MENU;
            player = Player();

            // Bullet first;
            // // Bullet second;
            // // Bullets.push_back(first); //test bullet
            // // Bullets.push_back(second);
            Bullets.push_back(new Bullet);

        // load shaders
            ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
        // configure shaders
            glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
                static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
            ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
            ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
            Shader shader = ResourceManager::GetShader("sprite");
        // set render-specific controls
            Renderer = new SpriteRenderer(shader);
        // load textures
            ResourceManager::LoadTexture("resources/yukko desk.png", true, "yukko");
            ResourceManager::LoadTexture("resources/bananaduck.png", true, "player");
            ResourceManager::LoadTexture("resources/awesomeface.png", true, "title_screen");



        }
        // game loop
        void ProcessInput(float dt){

            if (Keys[GLFW_KEY_ENTER] && State == GAME_MENU){
                State = GAME_ACTIVE;
            }

            //if wasd is pressed, then update player objects velocity to be in the corresponding direction
            // (x,y), where 0,0 is the top left corner of the screen, +x is to the right, +y is down
            
            player.velocity.x = 0;
            player.velocity.y = 0;


            //TODO: make it use unit vector and then multiply vector by our movespeed instead

            if (Keys[GLFW_KEY_W] && State == GAME_ACTIVE)
            {
                //do something
                player.velocity.y += -1*player.moveSpeed;
            }

             if (Keys[GLFW_KEY_A] && State == GAME_ACTIVE)
            {
                player.velocity.x += -1*player.moveSpeed;
            }

            if (Keys[GLFW_KEY_S] && State == GAME_ACTIVE)
            {
                 player.velocity.y += player.moveSpeed;
            }
            
            if (Keys[GLFW_KEY_D] && State == GAME_ACTIVE)
            {
                player.velocity.x += player.moveSpeed;
            }


        }
        void Update(float dt){


            //move player if they are not at the edge of the screen
            player.position.x += player.velocity.x * dt;
            player.position.y += player.velocity.y * dt;

            if(player.touchingLeftWall()) player.position.x = 0;
            if(player.touchingRightWall()) player.position.x = static_cast<float>(window_width)-player.size;
            if(player.touchingUpperWall()) player.position.y = 0;
            if(player.touchingLowerWall()) player.position.y = static_cast<float>(window_height)-player.size;

            //issue: players coords are top left, want it to be centered


            player.calculate_center_coords();
            if(!Bullets.empty()){
                for(int i = 0; i < Bullets.size(); i++){
                    if(player.isTouching(*Bullets[i])){
                        // State = GAME_LOSE; //if you touch a bullet you lose
                        State = GAME_MENU;
                    }
                }

            }
        }
        void Render(){

            // Texture2D myTexture;
            // myTexture = ResourceManager::GetTexture("yukko");
            // Renderer->DrawSprite(myTexture, glm::vec2(400.0f, 300.0f), glm::vec2(500.0f, 500.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

            if (State == GAME_ACTIVE){
                //draw player
                Texture2D duck = ResourceManager::GetTexture("player");
                Renderer->DrawSprite(duck, player.position, glm::vec2(player.size, player.size), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

                //draw bullets

                if (!Bullets.empty()){    
                    for(int i = 0; i < Bullets.size(); i++){
                        Texture2D bullet = ResourceManager::GetTexture("yukko");
                        Renderer->DrawSprite(bullet, Bullets[i]->position, glm::vec2(Bullets[i]->size, Bullets[i]->size), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                    }}

            }
            if(State == GAME_MENU){//draw title screen if in menu
                Texture2D face = ResourceManager::GetTexture("title_screen");
                Renderer->DrawSprite(face, glm::vec2(00.0f, 00.0f), glm::vec2(static_cast<float>(this->Width), static_cast<float>(this->Width)), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }






        }
        void ResetGame(){}
};

