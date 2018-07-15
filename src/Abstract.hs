{- |
Module          : $Header$
Description     : This is the abstract file for the Scheme compiler.
Copyright       : (c) Michael Buchel
License         : BSD3

Maintainer      : abuchel2@uwo.ca
Stability       : experimental
Portability     : portable

This is the abstract module, where all the abstract concepts are kept.
| -}
module Abstract where

import Data.Array
import Data.Complex
import Data.Ratio

-- | Data structure for holding Lisp values.
data LispVal = Atom String
	     | List [LispVal]
	     | DottedList [LispVal] LispVal
	     | Number Integer
	     | String String
	     | Bool Bool
	     | Character Char
	     | Float Double
	     | Ratio Rational
	     | Complex (Complex Double)
	     | Vector (Array Int LispVal)

-- | Instance of show for LispVal.
instance Show LispVal where
	show = showVal

-- | Seperates the list into words.
unwordsList :: [LispVal] -- ^ List to unword.
	-> String
unwordsList = unwords . map showVal

-- | Lisp value to string.
showVal :: LispVal -- ^ Expression we got.
	-> String
showVal (Atom name) = name
showVal (List x) = "(" ++ unwordsList x ++ ")"
showVal (DottedList x y) = "(" ++ unwordsList x ++ " . " ++ showVal y ++ ")"
showVal (String contents) = "\"" ++ contents ++ "\""
showVal (Bool True) = "#t"
showVal (Bool False) = "#f"
showVal (Number x) = show x
showVal (Character x) = show x
showVal (Float x) = show x
showVal (Ratio x) = show x
showVal (Complex x) = show x
showVal (Vector x) = show x
