{- |
Module          : $Header$
Description     : This is the parser file for the Scheme compiler.
Copyright       : (c) Michael Buchel
License         : BSD3

Maintainer      : abuchel2@uwo.ca
Stability       : experimental
Portability     : portable

This is the parser, where I am using Parsec for simplicity.
| -}
module Parser where

import Abstract
import Control.Monad
import Control.Monad.Except
import Data.Array
import Data.Complex
import Data.Ratio
import Error
import Evaluator
import Numbers
import Text.ParserCombinators.Parsec

-- | Symbol function which identifies all the symbols allowed in Scheme.
symbol :: Parser Char
symbol = oneOf "!$%&|*+-/:<=>?@^_~"

-- | Escaped character parser.
escapedChars :: Parser Char
escapedChars = do
	char '\\'
	x <- oneOf "\\\"nrt"
	return $ case x of
		      '\\' -> x
		      '"' -> x
		      'n' -> '\n'
		      'r' -> '\r'
		      't' -> '\t'

-- | Parses a string.
parseString :: Parser LispVal
parseString = do
	char '"'
	x <- many $ escapedChars <|> noneOf "\"\\"
	char '"'
	return $ String x

-- | Parses an atom.
parseAtom :: Parser LispVal
parseAtom = do
	first <- letter <|> symbol
	rest <- many (letter <|> digit <|> symbol)
	let atom = first:rest
	return $ Atom atom

-- | Parses a boolean value.
parseBool :: Parser LispVal
parseBool = do
	char '#'
	(char 't' >> return (Bool True)) <|> (char 'f' >> return (Bool False))

-- | Parses decimals without the identifier string.
parseDecimal1 :: Parser LispVal
parseDecimal1 = many1 digit >>= (return . Number . read)

-- | Parses decimals with the identifier string.
parseDecimal2 :: Parser LispVal
parseDecimal2 = do
	try $ string "#d"
	many1 digit >>= (return . Number . read)

-- | Parses hexadecimals.
parseHex :: Parser LispVal
parseHex = do
	try $ string "#x"
	many1 hexDigit >>= (return . Number . hex2dig)

-- | Parses octal numbers.
parseOct :: Parser LispVal
parseOct = do
	try $ string "#o"
	many1 octDigit >>= (return . Number . oct2dig)

-- | Parses binary numbers.
parseBin :: Parser LispVal
parseBin = do
	try $ string "#b"
	many1 (oneOf "10") >>= (return . Number . bin2dig)

-- | Parses a number.
parseNumber :: Parser LispVal
parseNumber = parseDecimal1 <|> parseDecimal2 <|> parseHex <|> parseOct <|> parseBin

-- | Parses a floating point number.
parseFloat :: Parser LispVal
parseFloat = do
	x <- many1 digit
	char '.'
	y <- many1 digit
	return $ Float $ getFloat x y

-- | Parses a ratio value.
parseRatio :: Parser LispVal
parseRatio = do
	x <- many1 digit
	spaces >> char '/' >> spaces
	y <- many1 digit
	return $ Ratio ((read x) % (read y))

-- | Parses a complex number.
parseComplex :: Parser LispVal
parseComplex = do
	x <- (try parseFloat <|> parseDecimal1)
	spaces >> char '+' >> spaces
	y <- (try parseFloat <|> parseDecimal1)
	oneOf "ij"
	return $ Complex (toDouble x :+ toDouble y)

-- | Helper list parser.
parseListHelp :: Parser LispVal
parseListHelp = do
	char '.' >> (skipMany1 space)
	x <- parseExpr
	spaces >> char ')'
	return x

-- | Parses an individual list.
parseList :: Parser LispVal
parseList = do
	char '(' >> spaces
	headList <- parseExpr `sepEndBy` (skipMany1 space)
	(parseListHelp >>= return . (DottedList headList)) <|> (spaces >> char ')' >> return (List headList))

-- | For Scheme syntax sugar.
parseQuoted :: Parser LispVal
parseQuoted = do
	char '\''
	x <- parseExpr
	return $ List [Atom "quote", x]

-- | For Scheme syntax sugar.
parseQuasiQuoted :: Parser LispVal
parseQuasiQuoted = do
	char '`'
	x <- parseExpr
	return $ List [Atom "quasiquote", x]

-- | For Scheme syntax sugar.
parseUnQuoted :: Parser LispVal
parseUnQuoted = do
	char ','
	x <- parseExpr
	return $ List [Atom "unquote", x]

-- | Parses a character value.
parseChar :: Parser LispVal
parseChar = do
	try $ string "#\\"
	value <- try (string "newline" <|> string "space") <|> do {x <- anyChar; notFollowedBy alphaNum; return [x]}
	return $ Character $ case value of
				  "space" -> ' '
				  "newline" -> '\n'
				  otherwise -> (value !! 0)

-- | Parses a vector.
parseVector :: Parser LispVal
parseVector = do
	arrayVals <- sepBy parseExpr spaces
	return $ Vector (listArray (0, (length arrayVals - 1)) arrayVals)

-- | Helper function for parsing vectors.
parseVectors :: Parser LispVal
parseVectors = do
	string "#(" >> spaces
	x <- parseVector
	spaces >> char ')'
	return x

-- | Helper parsing function which tries to parse the expression given to it.
parseExpr :: Parser LispVal
parseExpr = parseAtom
	<|> parseString
	<|> parseQuoted
	<|> parseQuasiQuoted
	<|> parseUnQuoted
	<|> try parseComplex
	<|> try parseFloat
	<|> try parseRatio
	<|> try parseNumber
	<|> try parseBool
	<|> try parseChar
	<|> try parseVectors
	<|> parseList

-- | A function to read the expression from a string.
readExpr :: String -- ^ Input string
	-> ThrowsError LispVal
readExpr input = case parse (spaces >> parseExpr) "parser" input of
		      Left err -> throwError $ Parser err
		      Right val -> return val

-- | Find the end of an expression.
findExpr :: String -- ^ Input string.
	-> Integer -- ^ Previous integer.
	-> Integer -- ^ Nesting of expression.
	-> Integer
findExpr [] x _ = x
findExpr (x : xs) n y = case x of
			     '(' -> findExpr xs (n + 1) (y + 1)
			     ')' -> if (y - 1) == 0 then (n + 1) else findExpr xs (n + 1) (y - 1)
			     otherwise -> findExpr xs (n + 1) y
