{- |
Module          : $Header$
Description     : This is the evaluator file for the Scheme compiler.
Copyright       : (c) Michael Buchel
License         : BSD3

Maintainer      : abuchel2@uwo.ca
Stability       : experimental
Portability     : portable

This is the evaluator module, where all evaluator specific things are kept.
| -}
module Evaluator where

import Abstract
import Control.Monad.Except
import Error
import EvalHelper
import Numbers

-- | Applies a unary function.
unaryOp :: (LispVal -> LispVal) -- ^ Function to apply.
	-> [LispVal] -- ^ Argument to the function.
	-> ThrowsError LispVal
unaryOp f [] = throwError $ NumArgs 1 []
unaryOp f [v] = return $ f v
unaryOp f x = throwError $ NumArgs 1 x

-- | Applies a numeric binary function to a list of lisp values.
numericBinop :: (Integer -> Integer -> Integer) -- ^ Function to apply.
	-> [LispVal] -- ^ Lisp values to apply it to.
	-> ThrowsError LispVal
numericBinop op [] = throwError $ NumArgs 2 []
numericBinop op val@[_] = throwError $ NumArgs 2 val
numericBinop op params = mapM unpackNum params >>= return . Number . foldl1 op

-- | Applies a boolean binary function.
boolBinop :: (LispVal -> ThrowsError a) -- ^ Unpacker function.
	-> (a -> a -> Bool) -- ^ Function to apply.
	-> [LispVal] -- ^ Arguments.
	-> ThrowsError LispVal
boolBinop unpacker op args = if length args /= 2
				then throwError $ NumArgs 2 args
				else do
					left <- unpacker $ args !! 0
					right <- unpacker $ args !! 1
					return $ Bool $ left `op` right

-- | Applies a different kind of numeric binary function.
numBoolBinop :: (Integer -> Integer -> Bool) -- ^ Function to apply.
	-> [LispVal] -- ^ Arguments.
	-> ThrowsError LispVal
numBoolBinop = boolBinop unpackNum

-- | For strings.
strBoolBinop :: (String -> String -> Bool) -- ^ Function to apply.
	-> [LispVal] -- ^ Arguments.
	-> ThrowsError LispVal
strBoolBinop = boolBinop unpackStr

-- | For booleans.
boolBoolBinop :: (Bool -> Bool -> Bool) -- ^ Function to apply.
	-> [LispVal] -- ^ Arguments.
	-> ThrowsError LispVal
boolBoolBinop = boolBinop unpackBool

-- | List of primative functions.
primatives :: [(String, [LispVal] -> ThrowsError LispVal)]
primatives = [
		("+", numericBinop (+)),
		("-", numericBinop (-)),
		("*", numericBinop (*)),
		("/", numericBinop (div)),
		("mod", numericBinop (mod)),
		("quot", numericBinop (quot)),
		("rem", numericBinop (rem)),
		("symbol?", unaryOp (symbolp)),
		("string?", unaryOp (stringp)),
		("number?", unaryOp (numberp)),
		("boolean?", unaryOp (boolp)),
		("list?", unaryOp (listp)),
		("=", numBoolBinop (==)),
		("<", numBoolBinop (<)),
		(">", numBoolBinop (>)),
		("/=", numBoolBinop (/=)),
		(">=", numBoolBinop (>=)),
		("<=", numBoolBinop (<=)),
		("&&", boolBoolBinop (&&)),
		("||", boolBoolBinop (||)),
		("string=?", strBoolBinop (==)),
		("string<?", strBoolBinop (<)),
		("string>?", strBoolBinop (>)),
		("string<=?", strBoolBinop (<=)),
		("string>=?", strBoolBinop (>=)),
		("car", car),
		("cdr", cdr),
		("cons", cons),
		("eq?", eqv),
		("eqv?", eqv),
		("equal?", equal)
	]

-- | Applies the function to a list of lisp values.
apply :: String -- ^ Function to apply.
	-> [LispVal] -- ^ Arguments to the function
	-> ThrowsError LispVal
apply func args = maybe (throwError $ NotFunction "Unrecognized primitive function args" func) ($ args) $ lookup func primatives

-- | Evaluates the expression given.
eval :: LispVal -- ^ LispVal to evaluate.
	-> ThrowsError LispVal
eval val@(Atom _) = return val
eval val@(String _) = return val
eval val@(Bool _) = return val
eval val@(Number _) = return val
eval val@(Character _) = return val
eval val@(Float _) = return val
eval val@(Ratio _) = return val
eval val@(Complex _) = return val
eval (List [Atom "quote", val]) = return val
eval (List [Atom "unquote", val]) = return val
eval (List [Atom "quasiquote", val]) = return val
eval (List [Atom "if", p, c, a]) = do
	res <- eval p
	case res of
	     Bool False -> eval a
	     otherwise -> eval c
eval (List (Atom func : args)) = mapM eval args >>= apply func
eval badForm = throwError $ BadSpecialForm "Unrecognized special form" badForm
