% File "bridge.t".
%... Declarations of constants and variables here ...
type cardType :
    record
        value : int
        suit : string (1)
    end record
const cardsInDeck := 52
const numberOfSuits := 4
const cardsInSuit := 13
const suit : array 1 .. 4 of string (1) := init ("S", "H", "D", "C")
var deck : array 1 .. cardsInDeck of cardType
type recordType : cardType
const numberOfHands := 4
const cardsInHand := 13
var hand : array 1 .. numberOfHands, 1 .. cardsInHand of cardType

% ... Definition of procedures here ...
procedure swap (i, j : int, var list : array 1 .. * of recordType)
    % Swap ith and jth record in list.
    pre i >= 0 and i <= upper (list) and j >= 0 and j <= upper (list)
    const temp : recordType := list (i)
    list (i) := list (j)
    list (j) := temp
end swap

procedure generateDeck
    var card := 1
    for whichSuit : 1 .. numberOfSuits
        for whichValue : 2 .. 14
            deck (card).value := whichValue
            deck (card).suit := suit (whichSuit)
            card := card + 1
        end for
    end for
end generateDeck

procedure shuffleDeck
    var where : int
    for whichCard : 1 .. cardsInDeck
        % Generate the random number between 1 and cardsInDeck.
        where := Rand.Int (1, cardsInDeck)
        swap (whichCard, where, deck)
    end for
end shuffleDeck

procedure dealHands
    % Deal one card to each hand in turn repeatedly.
    var card := 1
    for cardCount : 1 .. cardsInHand
        for handCount : 1 .. numberOfHands
            hand (handCount, cardCount) := deck (card)
            % Move to next card in deck.
            card += 1
        end for
    end for
end dealHands

procedure listHands
    % Hands are listed one card to a line, unsorted.
    for whichHand : 1 .. numberOfHands
        put "Here are the cards for hand ", whichHand
        for whichCard : 1 .. cardsInHand
            put " " : 4, hand (whichHand, whichCard).suit : 3,
                hand (whichHand, whichCard).value
        end for
    end for
end listHands

% ... Main program follows ...
generateDeck
shuffleDeck
dealHands
listHands
