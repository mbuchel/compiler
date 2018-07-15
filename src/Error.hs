{- |
Module          : $Header$
Description     : This is the error file for the Scheme compiler.
Copyright       : (c) Michael Buchel
License         : BSD3

Maintainer      : abuchel2@uwo.ca
Stability       : experimental
Portability     : portable

This is the error module, where all the error concepts are kept.
| -}
module Error where

import Abstract
import Control.Monad.Except
import Text.ParserCombinators.Parsec

-- | To help throwing errors.
type ThrowsError = Either LispError

-- | Error data structure for lisp.
data LispError = NumArgs Integer [LispVal]
	       | TypeMismatch String LispVal
	       | Parser ParseError
	       | BadSpecialForm String LispVal
	       | NotFunction String String
	       | UnboundVar String String
	       | Default String

-- | Instance of show for LispError.
instance Show LispError where
	show = showErr

-- | Lisp error to string.
showErr :: LispError -- ^ Error we got.
	-> String
showErr (UnboundVar message var) = message ++ ": " ++ var
showErr (BadSpecialForm message form) = message ++ ": " ++ show form
showErr (NotFunction message func) = message ++ ": " ++ func
showErr (NumArgs expected found) = "Expected " ++ show expected ++ " args, but found: " ++ unwordsList found
showErr (TypeMismatch expected found) = "Invalid expected " ++ expected ++ ". Found " ++ show found ++ " instead."
showErr (Parser parseErr) = "Parse error at " ++ show parseErr ++ "."

-- | Catches errors.
trapError :: (MonadError a m, Show a) => m String -- ^ Caught error.
	-> m String
trapError action = catchError action (return . show)

-- | Extracts the error value.
extractValue :: ThrowsError a -- ^ Error.
	-> a
extractValue (Right val) = val
