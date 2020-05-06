CREATE TABLE IF NOT EXISTS `User` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `UserName` varchar(255) DEFAULT NULL,
  `Password` varchar(128) DEFAULT NULL,
  `SSOKey` varchar(128) DEFAULT NULL,
  `SSOStamp` timestamp NULL DEFAULT utc_timestamp(),
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4;