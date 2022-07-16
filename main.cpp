#include"chess.h"

Vector2f offset(5,6);





int main()
{

#ifdef _OPENMP
 printf("OpenMP is supported!\n");
#endif

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sounds/makeMove.wav"))
        return -1;
    sf::Sound sound;
    sound.setBuffer(buffer);
    RenderWindow window(VideoMode(490, 490), "Chess");

    Texture t;
    t.loadFromFile("images/board4.png");

    Sprite sBoard(t);

    ChessBoard board(WHITE);
	int depthOfAnalysis = 2;//3???
	ChessAI computer(depthOfAnalysis, BLACK);

	board.loadPosition();
	Move computerMove;
	Move playerMove;


    bool isMove=false;
    bool isCompMove = false;
    float dx=0, dy=0;
    Vector2f oldPos,newPos;
    Piece* selectedPiece;

    while (window.isOpen())
    {
        Vector2i pos = Mouse::getPosition(window) - Vector2i(offset);


       //comp move
        if (isCompMove)
        {
            isCompMove = false;
            double time = -omp_get_wtime();

            computerMove = computer.findBestMoveKiller(board);

            time += omp_get_wtime();
            std::cout<<"total time = "<<time<<"\n";
            computerMove.show();

            newPos.x = computerMove.m_newPos[0]* SIZE_OF_CELL;
            newPos.y = computerMove.m_newPos[1]* SIZE_OF_CELL;
            oldPos.x = computerMove.m_oldPos[0]* SIZE_OF_CELL;
            oldPos.y = computerMove.m_oldPos[1]* SIZE_OF_CELL;

            selectedPiece = board.piecesOnBoard.at(computerMove.m_oldPos[0]*8+computerMove.m_oldPos[1]);
            selectedPiece->m_sprite.setPosition(newPos);

                    if(abs(board.getCell(computerMove.m_oldPos))==KING){
                        if(computerMove.m_newPos[0] - computerMove.m_oldPos[0]==2){
                            board.piecesOnBoard.at(7*8+computerMove.m_newPos[1])->m_sprite.setPosition(newPos+Vector2f(-SIZE_OF_CELL,0));
                        }
                    }

            board.makeMove(computerMove);
            board.showWithPos();
            sound.play();
        }
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            /////drag and drop///////
            if (e.type == Event::MouseButtonPressed)
                if (e.key.code == Mouse::Left){

                    for (auto& piece : board.piecesOnBoard) {
                        if (piece->m_sprite.getGlobalBounds().contains(pos.x,pos.y) && piece->getColor() == board.getPlayerColor())
                            {
                                isMove=true; selectedPiece=piece;
                                dx=pos.x - piece->m_sprite.getPosition().x;
                                dy=pos.y - piece->m_sprite.getPosition().y;
                                oldPos  =  piece->m_sprite.getPosition();
                                int x,y;
                                piece->getPos(x,y);
                                playerMove.m_oldPos[0] = x;
                                playerMove.m_oldPos[1] = y;
                      }
                    }


                }

             if (e.type == Event::MouseButtonReleased && isMove)
                if (e.key.code == Mouse::Left)
                 {
                  isMove=false;
                  Vector2f p = selectedPiece->m_sprite.getPosition() + Vector2f(SIZE_OF_CELL/2,SIZE_OF_CELL/2);
                  newPos = Vector2f( SIZE_OF_CELL*int(p.x/SIZE_OF_CELL), SIZE_OF_CELL*int(p.y/SIZE_OF_CELL) );

                  playerMove.m_newPos[0] = p.x/SIZE_OF_CELL;
                  playerMove.m_newPos[1] = p.y/SIZE_OF_CELL;

                  if(selectedPiece->isLegalMove(playerMove, board)){
                    if(abs(board.getCell(playerMove.m_oldPos))==KING){
                        if(playerMove.m_newPos[0] - playerMove.m_oldPos[0]==2){
                            board.piecesOnBoard.at(7*8+playerMove.m_newPos[1])->m_sprite.setPosition(newPos+Vector2f(-SIZE_OF_CELL,0));
                        }
                    }
                    selectedPiece->m_sprite.setPosition(newPos);
                    board.makeMove(playerMove);
                    isCompMove = true;
                  }
                  else{
                    selectedPiece->m_sprite.setPosition(oldPos);
                  }

                 }

        }



        if (isMove) selectedPiece->m_sprite.setPosition(pos.x-dx,pos.y-dy);

    ////// draw  ///////
        window.clear();
        window.draw(sBoard);
        for (auto& piece : board.piecesOnBoard) {
            piece->m_sprite.move(offset);
            window.draw(piece->m_sprite);
            piece->m_sprite.move(-offset);
        }
        if (isMove){
            selectedPiece->m_sprite.move(offset);
            window.draw(selectedPiece->m_sprite);
            selectedPiece->m_sprite.move(-offset);
        }

        window.display();
    }


    return 0;
}
