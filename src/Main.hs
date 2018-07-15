{- |
Module          : $Header$
Description     : This is the main source code for the Scheme compiler.
Copyright       : (c) Michael Buchel
License         : BSD3

Maintainer      : abuchel2@uwo.ca
Stability       : experimental
Portability     : portable

This is my attempt at making a Scheme compiler, in Haskell.
| -}
module Main where

import Control.Monad
import Data.String.Utils
import Error
import Evaluator
import Parser
import System.Environment
import System.IO

-- | Flushes the output buffer.
flushStr :: String -- ^ String to output.
	-> IO ()
flushStr str = putStr str >> hFlush stdout

-- | Reads the user input.
readPrompt :: String -- ^ Prompt string.
	-> IO String
readPrompt prompt = flushStr prompt >> getLine

-- | Evaluates the string given.
evalString :: String -- ^ String to evaluate.
	-> IO String
evalString expr = return $ extractValue $ trapError (liftM show $ readExpr expr >>= eval)

-- | Evaluates and prints the string.
evalAndPrint :: String -- ^ String to evaluate.
	-> IO ()
evalAndPrint expr = evalString expr >>= putStrLn

-- | Until for REPL.
until_ :: Monad m => (a -> Bool) -- ^ End condition.
	-> m a -- ^ Prompt function.
	-> (a -> m ()) -- ^ Output function
	-> m ()
until_ p pr act = do
	res <- pr
	if p res
	   then return ()
	   else act res >> until_ p pr act

-- | Runs the REPL function.
runRepl :: IO()
runRepl = until_ (== "quit") (readPrompt "scheme> ") evalAndPrint

-- | File eval and printing.
fileEvalAndPrint :: String -- ^ Input string.
	-> IO ()
fileEvalAndPrint [] = return ()
fileEvalAndPrint program = do
	let num = findExpr program 0 0
	let h = take (fromInteger num) program
	let n = strip $ drop (fromInteger num) program
	evalAndPrint h
	fileEvalAndPrint n

-- | Main function which interacts with the user and preforms IO.
main :: IO ()
main = do
	args <- getArgs
	case length args of
	     0 -> runRepl
	     1 -> readFile (head args) >>= fileEvalAndPrint
	     otherwise -> putStrLn "Program only takes 0 or 1 arguments"
