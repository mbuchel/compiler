{- |
Module          : $Header$
Description     : This is the numbers file for the Scheme compiler.
Copyright       : (c) Michael Buchel
License         : BSD3

Maintainer      : abuchel2@uwo.ca
Stability       : experimental
Portability     : portable

This is the number module for parsing numbers and so on.
| -}
module Numbers where

import Abstract
import Control.Monad.Except
import Error
import Numeric

-- | Octal to integer function.
oct2dig :: (Eq a, Num a) => String -- ^ Input string to the function.
	-> a
oct2dig x = fst $ readOct x !! 0

-- | Hexadecimal to integer function.
hex2dig :: (Eq a, Num a) => String -- ^ Input string to the function.
	-> a
hex2dig x = fst $ readHex x !! 0

-- | Helper binary to digit function.
bin2dig' :: Num t => t -- ^ Last known number.
	-> String -- ^ Binary string.
	-> t
bin2dig' digint "" = digint
bin2dig' digint (x:xs) = let old = 2 * digint + (if x == '0' then 0 else 1) in bin2dig' old xs

-- | Actual binary to digit function.
bin2dig :: Num t => String -- ^ Binary string.
	-> t
bin2dig = bin2dig' 0

-- | Gets a floating point number.
getFloat :: String -- ^ Before the decimal point.
	-> String -- ^ After the decimal point.
	-> Double
getFloat x y = fst . head $ readFloat (x ++ "." ++ y)

-- | Convert to double from LispVal.
toDouble :: LispVal -- ^ Value to convert.
	-> Double
toDouble (Float f) = realToFrac f
toDouble (Number n) = fromIntegral n

-- | Unpacks numbers from LispVal.
unpackNum :: LispVal -- ^ LispVal to unpack.
	-> ThrowsError Integer
unpackNum (Number n) = return n
unpackNum (String n) = let parsed = reads n in 
                           if null parsed 
                             then throwError $ TypeMismatch "number" $ String n
                             else return $ fst $ parsed !! 0
unpackNum (List [n]) = unpackNum n
unpackNum x = throwError $ TypeMismatch "number" x
