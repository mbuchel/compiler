{- |
Module          : $Header$
Description     : This is the eval helper file for the Scheme compiler.
Copyright       : (c) Michael Buchel
License         : BSD3

Maintainer      : abuchel2@uwo.ca
Stability       : experimental
Portability     : portable

This is the eval helper module, where all eval helper specific things are kept.
| -}
{-# LANGUAGE ExistentialQuantification #-}
module EvalHelper where

import Abstract
import Control.Monad.Except
import Error
import Numbers

-- | To help us in unpacking the lispval list for equal?.
data Unpacker = forall a. Eq a => AnyUnpacker (LispVal -> ThrowsError a)

-- | Checks if the input is an atom.
symbolp :: LispVal -- ^ Input value.
	-> LispVal
symbolp (Atom _) = Bool True
symbolp _ = Bool False

-- | Checks if the input is a number.
numberp :: LispVal -- ^ Input value.
	-> LispVal
numberp (Number _) = Bool True
numberp _ = Bool False

-- | Checks if the input is a string.
stringp :: LispVal -- ^ Input value.
	-> LispVal
stringp (String _) = Bool True
stringp _ = Bool False

-- | Checks if the input is a boolean.
boolp :: LispVal -- ^ Input value.
	-> LispVal
boolp (Bool _) = Bool True
boolp _ = Bool False

-- | Checks if the input is a list.
listp :: LispVal -- ^ Input value.
	-> LispVal
listp (List _) = Bool True
listp (DottedList _ _) = Bool True
listp _ = Bool False

-- | Unpacks a string.
unpackStr :: LispVal -- ^ Argument to unpack. 
	-> ThrowsError String
unpackStr (String s) = return s
unpackStr (Number s) = return $ show s
unpackStr (Bool s) = return $ show s
unpackStr s = throwError $ TypeMismatch "string" s

-- | Unpacks a boolean value.
unpackBool :: LispVal -- ^ Argument to unpack
	-> ThrowsError Bool
unpackBool (Bool b) = return b
unpackBool b = throwError $ TypeMismatch "boolean" b

-- | Car list primative.
car :: [LispVal] -- ^ Inputs.
	-> ThrowsError LispVal
car [List (x : xs)] = return x
car [DottedList (x : xs) _] = return x
car [x] = throwError $ TypeMismatch "pair" x
car x = throwError $ NumArgs 1 x

-- | Cdr list primative.
cdr :: [LispVal] -- ^ Inputs.
	-> ThrowsError LispVal
cdr [List (x : xs)] = return $ List xs
cdr [DottedList [_] x] = return x
cdr [DottedList (_ : xs) x] = return $ DottedList xs x
cdr [x] = throwError $ TypeMismatch "pair" x
cdr x = throwError $ NumArgs 1 x

-- | Cons list primative.
cons :: [LispVal] -- ^ Inputs.
	-> ThrowsError LispVal
cons [x, List []] = return $ List [x]
cons [x, List xs] = return $ List $ x : xs
cons [x, DottedList xs xlast] = return $ DottedList (x : xs) xlast
cons [x, y] = return $ DottedList [x] y
cons x = throwError $ NumArgs 2 x

-- | Eqv function.
eqv :: [LispVal] -- ^ Inputs.
	-> ThrowsError LispVal
eqv [(Bool arg1), (Bool arg2)] = return $ Bool $ arg1 == arg2
eqv [(Number arg1), (Number arg2)] = return $ Bool $ arg1 == arg2
eqv [(String arg1), (String arg2)] = return $ Bool $ arg1 == arg2
eqv [(Atom arg1), (Atom arg2)] = return $ Bool $ arg1 == arg2
eqv [(DottedList xs x), (DottedList ys y)] = eqv [List (xs ++ [x]), List (ys ++ [y])]
eqv [(List arg1), (List arg2)] = return $ Bool $ (length arg1 == length arg2) && (all eqvPair $ zip arg1 arg2)
	where eqvPair (x1, x2) = case eqv [x1, x2] of
		Left err -> False
		Right (Bool val) -> val
eqv [_, _] = return $ Bool False
eqv x = throwError $ NumArgs 2 x

-- | Helper function for equal?.
unpackEquals :: LispVal -- ^ First argument to unpack.
	-> LispVal -- ^ Second argument to unpack.
	-> Unpacker -- ^ Unpacker function.
	-> ThrowsError Bool
unpackEquals arg1 arg2 (AnyUnpacker unpacker) = do
	unpacked1 <- unpacker arg1
	unpacked2 <- unpacker arg2
	return $ unpacked1 == unpacked2
	`catchError` (const $ return False)

-- | Equals function for equal?.
equal :: [LispVal] -- ^ Input arguments.
	-> ThrowsError LispVal
equal [arg1, arg2] = do
	p <- liftM or $ mapM (unpackEquals arg1 arg2) [AnyUnpacker unpackNum, AnyUnpacker unpackStr, AnyUnpacker unpackBool]
	e <- eqv [arg1, arg2]
	return $ Bool $ (p || let (Bool x) = e in x)
equal x = throwError $ NumArgs 2 x
