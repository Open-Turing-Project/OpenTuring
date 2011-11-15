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

% Hands are listed in four lines each, one for each suit.
% Cards are listed in suits in decreasing order of value.
procedure listHandsInOrder
    % Declare an array of four sets, one for each suit.
    type valueSet : set of 2 .. 14
    var suits : array 1 .. numberOfSuits of valueSet
    for whichHand : 1 .. numberOfHands
        % Initialize valueSets for this hand to null.
        for whichSuit : 1 .. numberOfSuits
            suits (whichSuit) := valueSet ()
        end for
        % Put cards of hand into suits.
        for card : 1 .. cardsInHand
            bind var thisCard to hand (whichHand, card)
            case thisCard.suit of
                label "S" :
                    suits (1) := suits (1) + valueSet (thisCard.value)
                label "H" :
                    suits (2) := suits (2) + valueSet (thisCard.value)
                label "D" :
                    suits (3) := suits (3) + valueSet (thisCard.value)
                label "C" :
                    suits (4) := suits (4) + valueSet (thisCard.value)
            end case
        end for
        const suitName : array 1 .. numberOfSuits of string (1)
            := init ("S", "H", "D", "C")
        const valueName : array 2 .. 14 of string (2)
            := init ("2", "3", "4", "5", "6", "7", "8", "9", "10",
            "J", "Q", "K", "A")
        % List cards in hand, one line for each suit.
        put "Here are the cards in hand ", whichHand
        for whichSuit : 1 .. numberOfSuits
            put suitName (whichSuit) + " " ..
            % List cards in set in order of decreasing value.
            for decreasing value : 14 .. 2
                if value in suits (whichSuit) then
                    put valueName (value), " " ..
                end if
            end for
            put "" % Finish line of output.
        end for
    end for
end listHandsInOrder

% ... Main program follows ...
generateDeck
shuffleDeck
dealHands
listHandsInOrder
