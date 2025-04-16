TextureCache a ete stocke dans entity mais cela est modifiable 

Initialisation de l'audio manager :
AudioManager* audio = AudioManager::getInstance();
audio->initialize(); (utilise les chemins par defauts)

utilisation de l'audio manager:
audio->playMusic("background.mp3");
audio->loadSoundEffect("explosion", "explosion.mp3");
audio->playSoundEffect("explosion");


Voici un exemple d'implementation du TextureCache (si il y a des questions demandez moi, aussi il faut recreer une fonction AnimatedSprite si on utilise une spritesheet de format different)
class Player : public DestructibleObject, public IComposite
{
public:
    Player(IComposite* scene, const float& life, sf::Vector2f& positioninit)
        : DestructibleObject(scene, 100)
        , IComposite(scene)
        , m_velocity(270.0f)
    {
        positioninit = m_scene->getRoot()->getScene()->GetCenterWindow();

        m_shape = new RectangleSFML(48, 48, scene->getRoot()->getScene());

        sf::Texture& spriteTexture = m_scene->getRoot()->getScene()->getTexture()->getTexture("spritesheet.png");

        m_animatedSprite = new AnimatedSprite(positioninit, spriteTexture);

        m_direction = { 0, 0 };
        m_isMoving = false;
    }

    ~Player()
    {
        delete m_animatedSprite;
    }

    void ProcessInput(const sf::Event& event) override
    {
        if (event.type == sf::Event::KeyPressed)
        {
            bool dirChanged = false;

            switch (event.key.code)
            {
            case sf::Keyboard::Q:  
                m_direction = { -1, 0 };
                dirChanged = true;
                break;
            case sf::Keyboard::D:  
                m_direction = { 1, 0 };
                dirChanged = true;
                break;
            case sf::Keyboard::Z: 
                m_direction = { 0, -1 };
                dirChanged = true;
                break;
            case sf::Keyboard::S:  
                m_direction = { 0, 1 };
                dirChanged = true;
                break;
            default:
                break;
            }

            if (dirChanged) {
                m_isMoving = true;
                m_animatedSprite->SetDirection(m_direction);
            }

        }
        else if (event.type == sf::Event::KeyReleased)
        {
            bool resetDirection = true;


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Z) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                resetDirection = false;
            }

            if (resetDirection)
            {
                m_direction = { 0, 0 };
                m_isMoving = false;
                m_animatedSprite->SetDirection(m_direction); 
            }
        }
    }
