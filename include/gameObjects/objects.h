#pragma once
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace GameObjects {
    struct AbstractObject {
        virtual void update(gl2d::Renderer2D& renderer, bool show) = 0;
        virtual int getHeight() = 0;
        virtual int getWidth() = 0;
        virtual glm::vec2& getPos() = 0;
        virtual ~AbstractObject() = default; // Virtual destructor for proper cleanup
    };

    class Object : public AbstractObject {
    protected:
        glm::vec2 pos {0, 0};
        int width = 0, height = 0;
    public:
        Object() = default;
        virtual ~Object() = default;

        int getHeight() override {
            return height;
        }

        int getWidth() override {
            return width;
        }

        glm::vec2& getPos() override {
            return pos;
        }
    };

    class Frame : public Object {

        public:
            Frame (glm::vec2 pPos, int w, int h ) {
                width = w;
                height = h;
                pos = { pPos.x - width / 2, pPos.y - height / 2 };
            }

            void update(gl2d::Renderer2D& renderer, bool show) override {

                // Calculate dimensions and position for the inner rectangle
                int gap = 5;
                int innerWidth = width + gap;
                int innerHeight = height + gap;
                glm::vec2 innerPos = { pos.x - gap / 2, pos.y - gap / 2 };
                // Draw the rectangle
                if ( show ) {
                    renderer.renderRectangle({innerPos, innerWidth, innerHeight }, Colors_White);
                    renderer.renderRectangle({pos, width, height}, Colors_Black);
                }
            }

    };

    struct OneBlock{
        int size;  
        gl2d::Color4f color_;
        int x = 0;
        int y = 0;

        OneBlock(int s, gl2d::Color4f c) : size(s), color_(c) {}

        bool collidesLeftWall(int leftWallx) {
            return (leftWallx > x - size/2);
        }
        bool collidesRightWall(int rightWallx) {
            return (rightWallx < x + size/2);
        }
        bool collidesBottom(int bottom ) {
            return bottom < y + size/2;
        }

        void move( int xc, int yc) {
            x += xc;
            y += yc;
        }

        void setPos( int pX, int pY ) {
            x = pX;
            y = pY;
        }

        void draw( gl2d::Renderer2D& renderer, bool show ) {
            // Draw the outer rectangle
            glm::vec2 outerPos = { x, y };
            if ( show )
                renderer.renderRectangle({outerPos , size, size}, Colors_Transparent);

            // Calculate dimensions and position for the inner rectangle
            int gap = 2;
            int innerWidth = size - gap;
            int innerHeight = size - gap;
            glm::vec2 innerPos = { x + gap / 2, y + gap / 2 };

            // Draw the inner rectangle
            if ( show )
                renderer.renderRectangle({innerPos, innerWidth, innerHeight}, color_);  
        }
    };

    /**
     * @brief blockTypes are determined by its index:
     * 0 = I-Block
     * 1 = J-Block
     * 2 = L-Block
     * 3 = O-Black
     * 4 = S-Block
     * 5 = T-Block
     * 6 = Z-Block
     * 
     */
    class Block : public Object{
        private:
            int o_size_ = 10;
        public:
            gl2d::Color4f color_;
            int id = 0;
            std::vector<OneBlock> shape;
            int rotation = 0;
            Block() {}

            Block (glm::vec2 pPos, int oneblockSize, gl2d::Color4f color, int identifier = 0) {
                o_size_ = oneblockSize;
                width = o_size_;
                height = o_size_;
                color_ = color;
                pos = pPos;
                id = identifier;

                for( int i = 0; i < 4; i++ )
                    shape.emplace_back(OneBlock(o_size_, color));

                 switch( id ) {
                    case 0: {
                        for ( int i = 0; i < 4; i++ ) 
                            shape[i].setPos(pos.x + o_size_ * i, pos.y);
                        break;
                    };
                    case 1: {
                        for ( int i = 0; i < 3; i++ ) 
                            shape[i].setPos( pos.x + o_size_ * i, pos.y);
                        shape[3].setPos( pos.x, pos.y - o_size_);
                        break;
                    };
                    case 2: {
                        for ( int i = 0; i < 3; i++ ) 
                            shape[i].setPos( pos.x + o_size_ * i, pos.y);
                        shape[3].setPos( pos.x + o_size_ * 2, pos.y - o_size_);
                        break;
                    };
                    case 3: {
                        shape[0].setPos( pos.x, pos.y);
                        shape[1].setPos( pos.x + o_size_, pos.y);
                        shape[2].setPos( pos.x, pos.y - o_size_);
                        shape[3].setPos( pos.x + o_size_, pos.y - o_size_);
                        break;
                    };
                    case 4: {
                        shape[0].setPos( pos.x, pos.y);
                        shape[1].setPos( pos.x + o_size_, pos.y);
                        shape[2].setPos( pos.x + o_size_ , pos.y - o_size_);
                        shape[3].setPos( pos.x + o_size_ * 2, pos.y - o_size_);
                        break;
                    };
                    case 5: {
                        for ( int i = 0; i < 3; i++ ) 
                            shape[i].setPos( pos.x + o_size_ * i, pos.y);
                        shape[3].setPos( pos.x + o_size_, pos.y - o_size_);
                        break;
                    };
                    case 6: {
                        shape[0].setPos( pos.x, pos.y - o_size_);
                        shape[1].setPos( pos.x + o_size_, pos.y - o_size_);
                        shape[2].setPos( pos.x + o_size_, pos.y);
                        shape[3].setPos( pos.x + o_size_ * 2, pos.y);
                        break;
                    };
                }
                
            }

            void rotate() {
                if ( rotation < 3)
                    rotation += 1;
                else
                    rotation = 0;
                 switch( id ) {
                    case 0: {
                        if ( rotation == 0 || rotation == 2) {
                            for ( int i = 0; i < 4; i++ ) 
                                shape[i].setPos(pos.x + o_size_ * i, pos.y); 
                        }else {
                            for ( int i = 0; i < 4; i++ ) 
                              shape[i].setPos(pos.x + o_size_, pos.y - o_size_ * 2 + o_size_ * i);
                        }
                        
                        break;
                    };
                    case 1: {
                        switch(rotation) {
                            case 0: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_ * i, pos.y);
                                shape[3].setPos( pos.x, pos.y - o_size_);
                                break;
                            }
                            case 1: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_, pos.y - o_size_ + o_size_ * i);
                                shape[3].setPos( pos.x + o_size_ * 2, pos.y - o_size_);
                                break;
                            }
                            case 2: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_ * i, pos.y);
                                shape[3].setPos( pos.x + o_size_ * 2, pos.y + o_size_);
                                break;
                            }
                            case 3: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_, pos.y - o_size_ + o_size_ * i);
                                shape[3].setPos( pos.x, pos.y + o_size_);
                                break;
                                }
                        }
                        break;
                    case 2: {
                        switch(rotation) {
                            case 0: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_ * i, pos.y);
                                shape[3].setPos( pos.x + o_size_ * 2, pos.y - o_size_);
                                break;
                            }
                            case 1: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_, pos.y - o_size_ + o_size_ * i);
                                shape[3].setPos( pos.x + o_size_ * 2, pos.y + o_size_);
                                break;
                            }
                            case 2: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_ * i, pos.y);
                                shape[3].setPos( pos.x, pos.y + o_size_);
                                break;
                            }
                            case 3: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_, pos.y - o_size_ + o_size_ * i);
                                shape[3].setPos( pos.x, pos.y - o_size_);
                                break;
                                }
                        }
                        break;
                    };
                    case 3: {
                        break;
                    };
                    case 4: {
                        if ( rotation == 0 || rotation == 2 ) {
                            shape[0].setPos( pos.x, pos.y);
                            shape[1].setPos( pos.x + o_size_, pos.y);
                            shape[2].setPos( pos.x + o_size_ , pos.y - o_size_);
                            shape[3].setPos( pos.x + o_size_ * 2, pos.y - o_size_);
                        }else {
                            shape[0].setPos( pos.x, pos.y);
                            shape[1].setPos( pos.x, pos.y - o_size_);
                            shape[2].setPos( pos.x + o_size_ , pos.y);
                            shape[3].setPos( pos.x + o_size_, pos.y + o_size_);
                        }
                        break;
                    };
                    case 5: {
                        switch(rotation) {
                            case 0: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_ * i, pos.y);
                                shape[3].setPos( pos.x + o_size_, pos.y - o_size_);
                                break;
                            }
                            case 1: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_, pos.y - o_size_ + o_size_ * i);
                                shape[3].setPos( pos.x + o_size_ * 2, pos.y);
                                break;
                            }
                            case 2: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_ * i, pos.y);
                                shape[3].setPos( pos.x + o_size_, pos.y + o_size_);
                                break;
                            }
                            case 3: {
                                for ( int i = 0; i < 3; i++ ) 
                                    shape[i].setPos( pos.x + o_size_, pos.y - o_size_ + o_size_ * i);
                                shape[3].setPos( pos.x, pos.y);
                                break;
                            }
                        }
                        break;
                    };
                    case 6: {
                        if ( rotation == 0 || rotation == 2 ) {
                            shape[0].setPos( pos.x, pos.y - o_size_);
                            shape[1].setPos( pos.x + o_size_, pos.y - o_size_);
                            shape[2].setPos( pos.x + o_size_, pos.y);
                            shape[3].setPos( pos.x + o_size_ * 2, pos.y);
                        }else {
                            shape[0].setPos( pos.x, pos.y);
                            shape[1].setPos( pos.x, pos.y + o_size_);
                            shape[2].setPos( pos.x + o_size_ , pos.y);
                            shape[3].setPos( pos.x + o_size_, pos.y - o_size_);
                        }
                        
                        break;
                    };
                    }
                }
            }

            bool collidesOtherBlock(std::shared_ptr<Block> other, gl2d::Renderer2D& renderer) {
                update(renderer, false);
                for ( int i = 0; i < shape.size(); i++ ) {
                    for ( int j = 0; j < other->shape.size(); j++ ) {
                         // Check if the new coordinates of afterBlock overlap with otherBlock
                        bool xOverlap = shape[i].x == other->shape[j].x;
                        bool yOverlap = shape[i].y == other->shape[j].y;
                        if ( xOverlap && yOverlap )
                            return true;
                    }
                }
                return false;
            }

            bool collidesLeftWall(int leftWall) {
                for ( auto& elem : shape ) {
                    if ( elem.collidesLeftWall(leftWall) )
                        return true;
                }
                return false;
            }
            bool collidesRightWall(int rightWall) {
                for ( auto& elem : shape ) {
                    if ( elem.collidesRightWall(rightWall) )
                        return true;
                }
                return false;
            }
            bool collidesBottom(int bottom) {
                for ( auto& elem : shape ) {
                    if ( elem.collidesBottom(bottom) )
                        return true;
                }
                return false;
            }
            void move(int xc, int yx) {
                pos = {pos.x +xc, pos.y + yx};
                for (auto& oneblocks : shape )
                    oneblocks.move(xc, yx);
            }
            void update(gl2d::Renderer2D& renderer, bool show) override {
               for (auto& oneblocks : shape )
                    oneblocks.draw(renderer, show);
            }
    };

};

