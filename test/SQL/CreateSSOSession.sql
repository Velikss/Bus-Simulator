CREATE TABLE IF NOT EXISTS `Session` (
  `Key` varchar(36) NOT NULL,
  `User_Id` int(11) NOT NULL,
  `Ip` varchar(15) NOT NULL,
  `Stamp` timestamp NOT NULL DEFAULT utc_timestamp(),
  PRIMARY KEY (`Key`),
  KEY `User_Id` (`User_Id`),
  KEY `Ip` (`Ip`),
  CONSTRAINT `User_Id` FOREIGN KEY (`User_Id`) REFERENCES `User` (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;