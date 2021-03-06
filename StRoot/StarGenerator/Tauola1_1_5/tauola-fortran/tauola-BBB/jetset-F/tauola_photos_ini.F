C this file is created by hand from taumain.F
C actions: Remove routines: TAUDEM DECTES TAUFIL FILHEP
C add:     INIETC will not necesarily work fine ... 
C replace  TRALO4 
C rename INIPHY to INIPHX

      SUBROUTINE INIETC(jakk1,jakk2,itd,ifpho)
      COMMON / IDFC  / IDFF
      COMMON / TAURAD / XK0DEC,ITDKRC
      DOUBLE PRECISION            XK0DEC
      COMMON / JAKI   /  JAK1,JAK2,JAKP,JAKM,KTOM
      COMMON /PHOACT/ IFPHOT
      SAVE
C KTO=1 will denote tau+, thus :: IDFF=-15
          IDFF=-15
C XK0 for tau decays.
          XK0DEC=0.01
C radiative correction switch in tau --> e (mu) decays !
          ITDKRC=itd
C switches of tau+ tau- decay modes !!
          JAK1=jakk1
          JAK2=jakk2
C photos activation switch
          IFPHOT=IFPHO
      end

      SUBROUTINE TRALO4(KTOS,PHOI,PHOF,AM)
!! Corrected 11.10.96 (ZW) tralor for KORALW.
!! better treatment is to  cascade from tau rest-frame through W
!! restframe down to LAB. 
      COMMON / MOMDEC / Q1,Q2,P1,P2,P3,P4
      COMMON /TRALID/ idtra
      double precision Q1(4),Q2(4),P1(4),P2(4),P3(4),P4(4),P1QQ(4),P2QQ(4)
      double precision PIN(4),POUT(4),PBST(4),PBS1(4),QQ(4),PI
      double precision THET,PHI,EXE
      REAL*4 PHOI(4),PHOF(4)
      SAVE
      DATA PI /3.141592653589793238462643D0/
      AM=SQRT(ABS
     $   (PHOI(4)**2-PHOI(3)**2-PHOI(2)**2-PHOI(1)**2))
      idtra=KTOS
      DO K=1,4
       PIN(K)=PHOI(K)
       PHOF(K)=PHOI(K)
      ENDDO
!      write(*,*) idtra
      IF    (idtra.EQ.1) THEN
        DO K=1,4
         PBST(K)=P1(K)
         QQ(K)=Q1(K)
        ENDDO
      ELSEIF(idtra.EQ.2) THEN
        DO K=1,4
         PBST(K)=P2(K)
         QQ(K)=Q1(K)
        ENDDO
      ELSEIF(idtra.EQ.3) THEN
        DO K=1,4
         PBST(K)=P3(K)
         QQ(K)=Q2(K)
        ENDDO
      ELSE
        DO K=1,4
         PBST(K)=P4(K)
         QQ(K)=Q2(K)
        ENDDO
      ENDIF



        CALL BOSTDQ(1,QQ,PBST,PBST)
        CALL BOSTDQ(1,QQ,P1,P1QQ)
        CALL BOSTDQ(1,QQ,P2,P2QQ)
        PBS1(4)=PBST(4)
        PBS1(3)=SQRT(PBST(3)**2+PBST(2)**2+PBST(1)**2)
        PBS1(2)=0D0
        PBS1(1)=0D0 
        EXE=(PBS1(4)+PBS1(3))/DSQRT(PBS1(4)**2-PBS1(3)**2)
C for KTOS=1 boost is antiparallel to 4-momentum of P2. 
C restframes of tau+ tau- and 'first' frame of 'higgs' are all connected
C by boosts along z axis
       IF(KTOS.EQ.1)  EXE=(PBS1(4)-PBS1(3))/DSQRT(PBS1(4)**2-PBS1(3)**2)
        CALL BOSTD3(EXE,PIN,POUT)

C once in Z/gamma/Higgs rest frame we control further kinematics by P2QQ for KTOS=1,2
        THET=ACOS(P2QQ(3)/SQRT(P2QQ(3)**2+P2QQ(2)**2+P2QQ(1)**2))
        PHI=0D0
        PHI=ACOS(P2QQ(1)/SQRT(P2QQ(2)**2+P2QQ(1)**2))
        IF(P2QQ(2).LT.0D0) PHI=2*PI-PHI

        CALL ROTPOX(THET,PHI,POUT)
        CALL BOSTDQ(-1,QQ,POUT,POUT)
      DO K=1,4
       PHOF(K)=POUT(K)
      ENDDO
      END
      SUBROUTINE CHOICE(MNUM,RR,ICHAN,PROB1,PROB2,PROB3,
     $            AMRX,GAMRX,AMRA,GAMRA,AMRB,GAMRB)
      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
C
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
C
      AMROP=1.1
      GAMROP=0.36
      AMOM=.782
      GAMOM=0.0084
C     XXXXA CORRESPOND TO S2 CHANNEL !
      IF(MNUM.EQ.0) THEN
       PROB1=0.5
       PROB2=0.5
       AMRX =AMA1
       GAMRX=GAMA1
       AMRA =AMRO
       GAMRA=GAMRO
       AMRB =AMRO
       GAMRB=GAMRO
      ELSEIF(MNUM.EQ.1) THEN
       PROB1=0.5
       PROB2=0.5
       AMRX =1.57
       GAMRX=0.9
       AMRB =AMKST
       GAMRB=GAMKST
       AMRA =AMRO
       GAMRA=GAMRO
      ELSEIF(MNUM.EQ.2) THEN
       PROB1=0.5
       PROB2=0.5
       AMRX =1.57
       GAMRX=0.9
       AMRB =AMKST
       GAMRB=GAMKST
       AMRA =AMRO
       GAMRA=GAMRO
      ELSEIF(MNUM.EQ.3) THEN
       PROB1=0.5
       PROB2=0.5
       AMRX =1.27
       GAMRX=0.3
       AMRA =AMKST
       GAMRA=GAMKST
       AMRB =AMKST
       GAMRB=GAMKST
      ELSEIF(MNUM.EQ.4) THEN
       PROB1=0.5
       PROB2=0.5
       AMRX =1.27
       GAMRX=0.3
       AMRA =AMKST
       GAMRA=GAMKST
       AMRB =AMKST
       GAMRB=GAMKST
      ELSEIF(MNUM.EQ.5) THEN
       PROB1=0.5
       PROB2=0.5
       AMRX =1.27
       GAMRX=0.3
       AMRA =AMKST
       GAMRA=GAMKST
       AMRB =AMRO
       GAMRB=GAMRO
      ELSEIF(MNUM.EQ.6) THEN
       PROB1=0.4
       PROB2=0.4
       AMRX =1.27
       GAMRX=0.3
       AMRA =AMRO
       GAMRA=GAMRO
       AMRB =AMKST
       GAMRB=GAMKST
      ELSEIF(MNUM.EQ.7) THEN
       PROB1=0.0
       PROB2=1.0
       AMRX =1.27
       GAMRX=0.9
       AMRA =AMRO
       GAMRA=GAMRO
       AMRB =AMRO
       GAMRB=GAMRO
      ELSEIF(MNUM.EQ.8) THEN
       PROB1=0.0
       PROB2=1.0
       AMRX =AMROP
       GAMRX=GAMROP
       AMRB =AMOM
       GAMRB=GAMOM
       AMRA =AMRO
       GAMRA=GAMRO
      ELSEIF(MNUM.EQ.9) THEN
       PROB1=0.5
       PROB2=0.5
       AMRX =AMA1
       GAMRX=GAMA1
       AMRA =AMRO
       GAMRA=GAMRO
       AMRB =AMRO
       GAMRB=GAMRO
      ELSEIF(MNUM.EQ.101) THEN
       PROB1=.35
       PROB2=.35
       AMRX =1.2
       GAMRX=.46
       AMRB =AMOM
       GAMRB=GAMOM
       AMRA =AMOM
       GAMRA=GAMOM
      ELSEIF(MNUM.EQ.102) THEN
       PROB1=0.0
       PROB2=0.0
       AMRX =1.4
       GAMRX=.6
       AMRB =AMOM
       GAMRB=GAMOM
       AMRA =AMOM
       GAMRA=GAMOM
      ELSEIF(MNUM.GE.103.AND.MNUM.LE.112) THEN
       PROB1=0.0
       PROB2=0.0
       AMRX =1.4
       GAMRX=.6
       AMRB =AMOM
       GAMRB=GAMOM
       AMRA =AMOM
       GAMRA=GAMOM


      ELSE
       PROB1=0.0
       PROB2=0.0
       AMRX =AMA1
       GAMRX=GAMA1
       AMRA =AMRO
       GAMRA=GAMRO
       AMRB =AMRO
       GAMRB=GAMRO
      ENDIF
C
      IF    (RR.LE.PROB1) THEN
       ICHAN=1
      ELSEIF(RR.LE.(PROB1+PROB2)) THEN
       ICHAN=2
        AX   =AMRA
        GX   =GAMRA
        AMRA =AMRB
        GAMRA=GAMRB
        AMRB =AX
        GAMRB=GX
        PX   =PROB1
        PROB1=PROB2
        PROB2=PX
      ELSE
       ICHAN=3
      ENDIF
C
      PROB3=1.0-PROB1-PROB2
      END
      SUBROUTINE INITDK
* ----------------------------------------------------------------------
*     INITIALISATION OF TAU DECAY PARAMETERS  and routines
*
*     called by : KORALZ
* ----------------------------------------------------------------------

      COMMON / DECPAR / GFERMI,GV,GA,CCABIB,SCABIB,GAMEL
      REAL*4            GFERMI,GV,GA,CCABIB,SCABIB,GAMEL
      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
*
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
      COMMON / TAUBRA / GAMPRT(500),JLIST(500),NCHAN
      COMMON / TAUKLE / BRA1,BRK0,BRK0B,BRKS
      REAL*4            BRA1,BRK0,BRK0B,BRKS

      PARAMETER (NMODE=86,NM1=0,NM2=11,NM3=19,NM4=22,NM5=21,NM6=13)
      COMMON / TAUDCD /IDFFIN(9,NMODE),MULPIK(NMODE)
     &                ,NAMES
      CHARACTER NAMES(NMODE)*31

      CHARACTER OLDNAMES(7)*31
      CHARACTER*80 bxINIT
      PARAMETER (
     $  bxINIT ='(1x,1h*,g17.8,            16x, a31,a4,a4, 1x,1h*)'
     $ )
      REAL*4 PI,POL1(4)
*
*
* LIST OF BRANCHING RATIOS
CAM normalised to e nu nutau channel
CAM                  enu   munu   pinu  rhonu   A1nu   Knu    K*nu   pi
CAM   DATA JLIST  /    1,     2,     3,     4,     5,     6,     7,

*AM   DATA GAMPRT /1.000,0.9730,0.6054,1.2432,0.8432,0.0432,O.O811,0.616
*AM
*AM  multipion decays
*
*    conventions of particles names
*                 K-,P-,K+,  K0,P-,KB,  K-,P0,K0
*                  3, 1,-3  , 4, 1,-4  , 3, 2, 4  ,
*                 P0,P0,K-,  K-,P-,P+,  P-,KB,P0
*                  2, 2, 3  , 3, 1,-1  , 1,-4, 2  ,
*                 ET,P-,P0 , P-,P0,GM  , P-,P0,P0
*                  9, 1, 2  , 1, 2, 8  ,  1, 2, 2
*

C
      DIMENSION NOPIK(9,NMODE),NPIK(NMODE)
*AM   outgoing multiplicity and flavors of multi-pion /multi-K modes    
      DATA   NPIK  /                4,                    4,    ! old 4scalar
     a                              4,                    4,    ! new (may 2004)
     b                              4,                    4,
     c                              4,                    4,
     d                              4,                    4,
     e                              4,                    4,    ! new (may 2004)
     e                              4,                    4,    ! new (sep 2004)
     e                              4,                    4,    
     e                              4,                    4,    
     e                              4,                    4,    
     e                              4,                    4,    ! new (sep 2004)
     1                              5,       
     a                              5,                    5,    ! new (may 2004)
     b                              5,                    5,
     c                              5,                    5,
     d                              5,                    5,
     e                              5,                    5,    ! new (may 2004)
     a                              5,                    5,    ! new (sep 2004)
     b                              5,                    5,
     c                              5,                    5,
     d                              5,                    5,
     e                              5,                    5,    ! new (sep 2004)
     x                                                    5,    ! old npi starts here
     2                              6,                    6,
     a                              6,                    6,    ! new (may 2004)
     b                              6,                    6,    ! new (may 2004)
     c                              6,                    6,    ! new (may 2004)
     d                              6,                    6,    ! new (may 2004)
     e                              6,                    6,    ! new (may 2004)
     3                              3,                    3,            
     4                              3,                    3,            
     5                              3,                    3,            
     6                              3,                    3,  
     7                              3,                          ! new (may 2004) and useful
     a                              3,                    3,    ! new (may 2004)
     a                              3,                    3,    ! new (may 2004)
     a                              3,                    3,    ! new (may 2004)
     a                              3,                    3,    ! new (may 2004)
     a                              3,                    3,    ! new (may 2004)
     8                                                    2, 
     9                              2,                    2,    ! new (may 2004)
     9                              2,                    2,    ! new (may 2004)
     9                              2,                    2,    ! new (may 2004)
     9                              2,                    2,    ! new (may 2004)
     9                              2,                    2/    ! new (may 2004)          

      DATA  NOPIK / -1,-1, 1, 2, 0, 0,3*0,     2, 2, 2,-1, 0, 0,3*0,  
     a               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (may 2004)
     b               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (may 2004)
     c               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (may 2004)
     d               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (may 2004)
     e               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (may 2004)
     a               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (sep 2004)
     b               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (sep 2004)
     c               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (sep 2004)
     d               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (sep 2004)
     e               4, 2, 2,-1, 0, 0,3*0,     4, 2, 2,-1, 0, 0,3*0,     ! new (sep 2004)
     1              -1,-1, 1, 2, 2, 0,3*0,  
     a              -1,-1, 1, 2, 2, 0,3*0,     2, 2, 2, 2, 2, 0,3*0,     ! new (may 2004)
     a               1,-1,-1, 2, 2, 0,3*0,    -1, 2, 2, 2, 2, 0,3*0,     ! new (may 2004)
     a              -1, 1, 1,-1,-1, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (may 2004)
     a              -1,-1, 1, 2, 4, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (may 2004)
     a              -1,-1, 1, 2, 4, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (may 2004)
     a              -1,-1, 1, 2, 4, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (sep 2004)
     a              -1,-1, 1, 2, 4, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (sep 2004)
     a              -1,-1, 1, 2, 4, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (sep 2004)
     a              -1,-1, 1, 2, 4, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (sep 2004)
     a              -1,-1, 1, 2, 4, 0,3*0,    -1,-1, 1, 2, 4, 0,3*0,     ! new (sep 2004)
     x                                        -1,-1,-1, 1, 1, 0,3*0,     ! old npi starts here
     2              -1,-1,-1, 1, 1, 2,3*0,    -1,-1, 1, 2, 2, 2,3*0, 
     a              -1,-1,-1, 1, 1, 1,3*0,    -1,-1, 1, 2, 2, 1,3*0,     ! new (may 2004)
     b              -1,-1,-1, 1, 1, 1,3*0,    -1,-1, 1, 2, 2, 1,3*0,     ! new (may 2004)
     c              -1,-1,-1, 1, 1, 1,3*0,    -1,-1, 1, 2, 2, 1,3*0,     ! new (may 2004)
     d              -1,-1,-1, 1, 1, 1,3*0,    -1,-1, 1, 2, 2, 1,3*0,     ! new (may 2004)
     e              -1,-1,-1, 1, 1, 1,3*0,    -1,-1, 1, 2, 2, 1,3*0,     ! new (may 2004)
     3              -3,-1, 3, 0, 0, 0,3*0,    -4,-1, 4, 0, 0, 0,3*0,  
     4              -3, 2,-4, 0, 0, 0,3*0,     2, 2,-3, 0, 0, 0,3*0,  
     5              -3,-1, 1, 0, 0, 0,3*0,    -1, 4, 2, 0, 0, 0,3*0,  
     6               9,-1, 2, 0, 0, 0,3*0,    -1, 2, 8, 0, 0, 0,3*0,


C AJWMOD fix sign bug, 2/22/99
     7               2, 2,-1, 0, 0, 0,3*0,                           ! new (may 2004) but useful
     7               2, 2, 2, 0, 0, 0,3*0,     2, 2, 2, 0, 0, 0,3*0, ! new (may 2004)
     7               2, 2, 2, 0, 0, 0,3*0,     2, 2, 2, 0, 0, 0,3*0, ! new (may 2004)
     7               2, 2, 2, 0, 0, 0,3*0,     2, 2, 2, 0, 0, 0,3*0, ! new (may 2004)
     7               2, 2, 2, 0, 0, 0,3*0,     2, 2, 2, 0, 0, 0,3*0, ! new (may 2004)
     7               2, 2, 2, 0, 0, 0,3*0,     2, 2, 2, 0, 0, 0,3*0, ! new (may 2004)

     8                                        -3,-4, 0, 0, 0, 0,3*0,
     8               -3,-3, 0, 0, 0, 0,3*0,   -3,-3, 0, 0, 0, 0,3*0, ! new (may 2004)
     8               -3,-3, 0, 0, 0, 0,3*0,   -3,-3, 0, 0, 0, 0,3*0, ! new (may 2004)
     8               -3,-3, 0, 0, 0, 0,3*0,   -3,-3, 0, 0, 0, 0,3*0, ! new (may 2004)
     8               -3,-3, 0, 0, 0, 0,3*0,   -3,-3, 0, 0, 0, 0,3*0, ! new (may 2004)
     8               -3,-3, 0, 0, 0, 0,3*0,   -3,-3, 0, 0, 0, 0,3*0 /! new (may 2004)


* LIST OF BRANCHING RATIOS
      NCHAN = NMODE + 7
      DO 1 I = 1,500
      IF (I.LE.NCHAN) THEN
        JLIST(I) = I

        IF(I.EQ. 1) GAMPRT(I) =0.1800 
        IF(I.EQ. 2) GAMPRT(I) =0.1751 
        IF(I.EQ. 3) GAMPRT(I) =0.1110 
        IF(I.EQ. 4) GAMPRT(I) =0.2515 
        IF(I.EQ. 5) GAMPRT(I) =0.1790 /2
        IF(I.EQ. 6) GAMPRT(I) =0.0071 
        IF(I.EQ. 7) GAMPRT(I) =0.0134
        IF(I.EQ. 8) GAMPRT(I) =0.0450
        IF(I.EQ. 9) GAMPRT(I) =0.0100

        IF(I.EQ.30) GAMPRT(I) =0.0009
        IF(I.EQ.33) GAMPRT(I) =0.004
        IF(I.EQ.34) GAMPRT(I) =0.002
        IF(I.EQ.35) GAMPRT(I) =0.001

        IF(I.EQ.51) GAMPRT(I) =0.0004 
        IF(I.EQ.52) GAMPRT(I) =0.0003 
        IF(I.EQ.53) GAMPRT(I) =0.0005 

        IF(I.EQ.64) GAMPRT(I) =0.0015 
        IF(I.EQ.65) GAMPRT(I) =0.0015 
        IF(I.EQ.66) GAMPRT(I) =0.0015 
        IF(I.EQ.67) GAMPRT(I) =0.0005
        IF(I.EQ.68) GAMPRT(I) =0.0050
        IF(I.EQ.69) GAMPRT(I) =0.0055
        IF(I.EQ.70) GAMPRT(I) =0.0017 
        IF(I.EQ.71) GAMPRT(I) =0.0013
        IF(I.EQ.72) GAMPRT(I) =0.1790 /2  

        IF(I.EQ.83) GAMPRT(I) =0.0010 

        IF(I.EQ. 1) OLDNAMES(I)='  TAU-  -->   E-               '
        IF(I.EQ. 2) OLDNAMES(I)='  TAU-  -->  MU-               '
        IF(I.EQ. 3) OLDNAMES(I)='  TAU-  -->  PI-               '
        IF(I.EQ. 4) OLDNAMES(I)='  TAU-  -->  PI-, PI0          '
        IF(I.EQ. 5) OLDNAMES(I)='  TAU-  -->  PI-, PI-,  PI+    '
        IF(I.EQ. 6) OLDNAMES(I)='  TAU-  -->   K-               '
        IF(I.EQ. 7) OLDNAMES(I)='  TAU-  -->  K*- (two subch)   '
        IF(I.EQ. 8) NAMES(I-7)='  TAU-  --> 2PI-,  PI0,  PI+   '
        IF(I.EQ. 9) NAMES(I-7)='  TAU-  --> 3PI0,        PI-   '

        IF(I.EQ.10) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.11) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.12) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.13) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.14) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.15) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.16) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.17) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.18) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)  
        IF(I.EQ.19) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (may 2004)
        IF(I.EQ.20) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.21) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.22) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.23) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.24) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.25) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.26) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.27) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)
        IF(I.EQ.28) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)  
        IF(I.EQ.29) NAMES(I-7)='  TAU-  --> xxxxxxx4xxxxxxxx   '  !  (sep 2004)


        IF(I.EQ.30) NAMES(I-7)='  TAU-  --> 2PI-, PI+, 2PI0 old'
        IF(I.EQ.31) NAMES(I-7)='  TAU-  --> a1 --> rho omega   '  !  (may 2004)
        IF(I.EQ.32) NAMES(I-7)='  TAU-  --> benchmark curr     '  !  (may 2004)
        IF(I.EQ.33) NAMES(I-7)='  TAU-  --> 2PI0, 2PI-,  PI+   '  !  (may 2004)
        IF(I.EQ.34) NAMES(I-7)='  TAU-  --> PI- 4PI0           '  !  (may 2004)
        IF(I.EQ.35) NAMES(I-7)='  TAU-  --> 3PI- 2PI+          '  !  (may 2004)
        IF(I.EQ.36) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (may 2004)
        IF(I.EQ.37) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (may 2004)
        IF(I.EQ.38) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (may 2004)
        IF(I.EQ.39) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (may 2004)
        IF(I.EQ.40) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (may 2004)

        IF(I.EQ.41) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.42) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.43) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.44) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.45) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.46) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.47) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.48) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.49) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)
        IF(I.EQ.50) NAMES(I-7)='  TAU-  --> xxxxxxxxx5xxxxxx   '  !  (sep 2004)

        IF(I.EQ.51) NAMES(I-7)='  TAU-  --> 3PI-, 2PI+,        '
        IF(I.EQ.52) NAMES(I-7)='  TAU-  --> 3PI-, 2PI+,  PI0   '
        IF(I.EQ.53) NAMES(I-7)='  TAU-  --> 2PI-,  PI+, 3PI0   '
        IF(I.EQ.54) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.55) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.56) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.57) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.58) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.59) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.60) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.61) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.62) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)
        IF(I.EQ.63) NAMES(I-7)='  TAU-  --> xxxxxxxxxnxxxxxx   '  !  (may 2004)

        IF(I.EQ.64) NAMES(I-7)='  TAU-  -->  K-, PI-,  K+      '
        IF(I.EQ.65) NAMES(I-7)='  TAU-  -->  K0, PI-, K0B      '

        IF(I.EQ.66) NAMES(I-7)='  TAU-  -->  K-,  K0, PI0      '

        IF(I.EQ.67) NAMES(I-7)='  TAU-  --> PI0  PI0   K-      '
        IF(I.EQ.68) NAMES(I-7)='  TAU-  -->  K-  PI-  PI+      '
        IF(I.EQ.69) NAMES(I-7)='  TAU-  --> PI-  K0B  PI0      '
        IF(I.EQ.70) NAMES(I-7)='  TAU-  --> ETA  PI-  PI0      '
        IF(I.EQ.71) NAMES(I-7)='  TAU-  --> PI-  PI0  GAM      '
        IF(I.EQ.72) NAMES(I-7)='  TAU-  --> PI-  PI0  PI0      '
        IF(I.EQ.73) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.74) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.75) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.76) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.77) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.78) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.79) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.80) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.81) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
        IF(I.EQ.82) NAMES(I-7)='  TAU-  --> xxxxxxxxx3xxxxxx   '  !  (may 2004)
 

        IF(I.EQ.83) NAMES(I-7)='  TAU-  -->  K-  K0            '
        IF(I.EQ.84) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.85) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.86) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.87) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.88) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.89) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.90) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.91) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.92) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)
        IF(I.EQ.93) NAMES(I-7)='  TAU-  --> xxxxxxxxx2xxxxxx   '  !  (may 2004)

      ELSE
        JLIST(I) = 0
        GAMPRT(I) = 0.
      ENDIF
   1  CONTINUE
      DO I=1,NMODE
        MULPIK(I)=NPIK(I)
        DO J=1,MULPIK(I)
         IDFFIN(J,I)=NOPIK(J,I)
        ENDDO
      ENDDO
        DO I=1,NCHAN
         GAMPRT(I) = 1D0/NCHAN
        ENDDO
          gamprt(31)=gamprt(31)*0.001
          gamprt(32)=gamprt(32)*0.001
        do k=1,10  ! these are brs for empty slots prepared for new channels 
          gamprt(36+k)=gamprt(36+k)*0.001
          gamprt(30-k)=gamprt(30-k)*0.001
          gamprt(30+10+k)=gamprt(30+10+k)*0.001
          gamprt(30-10-k)=gamprt(30-10-k)*0.001

          gamprt(53+k)=gamprt(53+k)*0.001
          gamprt(72+k)=gamprt(72+k)*0.001
          gamprt(83+k)=gamprt(83+k)*0.001
        enddo
         GAMPRT(72)=GAMPRT(72)/2
         GAMPRT(5)=GAMPRT(5)/2

*
*
* --- COEFFICIENTS TO FIX RATIO OF:
* --- A1 3CHARGED/ A1 1CHARGED 2 NEUTRALS MATRIX ELEMENTS (MASLESS LIM.)
* --- PROBABILITY OF K0 TO BE KS
* --- PROBABILITY OF K0B TO BE KS
* --- RATIO OF COEFFICIENTS FOR K*--> K0 PI-
* --- ALL COEFFICENTS SHOULD BE IN THE RANGE (0.0,1.0)
* --- THEY MEANING IS PROBABILITY OF THE FIRST CHOICE ONLY IF ONE
* --- NEGLECTS MASS-PHASE SPACE EFFECTS
      BRA1=1D0 ! 0.5
      BRK0=0.5
      BRK0B=0.5
      BRKS=0.6667
*

      GFERMI = 1.16637E-5
      CCABIB = 0.975
      GV     = 1.0
      GA     =-1.0



* ZW 13.04.89 HERE WAS AN ERROR
      SCABIB = SQRT(1.-CCABIB**2)
      PI =4.*ATAN(1.)
      GAMEL  = GFERMI**2*AMTAU**5/(192*PI**3)
*
      CALL DEXAY(-1,pol1)
*
      RETURN
      END
      FUNCTION DCDMAS(IDENT)
      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
*
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
      IF      (IDENT.EQ. 1) THEN
        APKMAS=AMPI
      ELSEIF  (IDENT.EQ.-1) THEN
        APKMAS=AMPI
      ELSEIF  (IDENT.EQ. 2) THEN
        APKMAS=AMPIZ
      ELSEIF  (IDENT.EQ.-2) THEN
        APKMAS=AMPIZ
      ELSEIF  (IDENT.EQ. 3) THEN
        APKMAS=AMK
      ELSEIF  (IDENT.EQ.-3) THEN
        APKMAS=AMK
      ELSEIF  (IDENT.EQ. 4) THEN
        APKMAS=AMKZ
      ELSEIF  (IDENT.EQ.-4) THEN
        APKMAS=AMKZ
      ELSEIF  (IDENT.EQ. 8) THEN
        APKMAS=0.0001
      ELSEIF  (IDENT.EQ.-8) THEN
        APKMAS=0.0001
      ELSEIF  (IDENT.EQ. 9) THEN
        APKMAS=0.5488
      ELSEIF  (IDENT.EQ.-9) THEN
        APKMAS=0.5488
      ELSE
        PRINT *, 'STOP IN APKMAS, WRONG IDENT=',IDENT
        STOP
      ENDIF
      DCDMAS=APKMAS
      END
      FUNCTION LUNPIK(ID,ISGN)
      COMMON / TAUKLE / BRA1,BRK0,BRK0B,BRKS
      REAL*4            BRA1,BRK0,BRK0B,BRKS
      REAL*4 XIO(1)
      IDENT=ID*ISGN

      IF      (IDENT.EQ. 1) THEN
        IPKDEF=-211
      ELSEIF  (IDENT.EQ.-1) THEN
        IPKDEF= 211
      ELSEIF  (IDENT.EQ. 2) THEN
        IPKDEF=111
      ELSEIF  (IDENT.EQ.-2) THEN
        IPKDEF=111
      ELSEIF  (IDENT.EQ. 3) THEN
        IPKDEF=-321
      ELSEIF  (IDENT.EQ.-3) THEN
        IPKDEF= 321

      ELSEIF  (IDENT.EQ. 4) THEN
*
* K0 --> K0_LONG (IS 130) / K0_SHORT (IS 310) = 1/1
        CALL RANMAR(XIO,1)
        IF (XIO(1).GT.BRK0) THEN
          IPKDEF= 130
        ELSE
          IPKDEF= 310
        ENDIF
      ELSEIF  (IDENT.EQ.-4) THEN
*
* K0B--> K0_LONG (IS 130) / K0_SHORT (IS 310) = 1/1
        CALL RANMAR(XIO,1)
        IF (XIO(1).GT.BRK0B) THEN
          IPKDEF= 130
        ELSE
          IPKDEF= 310
        ENDIF
      ELSEIF  (IDENT.EQ. 8) THEN
        IPKDEF= 22
      ELSEIF  (IDENT.EQ.-8) THEN
        IPKDEF= 22
      ELSEIF  (IDENT.EQ. 9) THEN
        IPKDEF= 221
      ELSEIF  (IDENT.EQ.-9) THEN
        IPKDEF= 221
      ELSE
        PRINT *, 'STOP IN IPKDEF, WRONG IDENT=',IDENT
        STOP
      ENDIF
      LUNPIK=IPKDEF
      END




      SUBROUTINE TAURDF(KTO)
C THIS ROUTINE CAN BE CALLED BEFORE ANY TAU+ OR TAU- EVENT IS GENERATED
C IT CAN BE USED TO GENERATE TAU+ AND TAU- SAMPLES OF DIFFERENT
C CONTENTS
      COMMON / TAUKLE / BRA1,BRK0,BRK0B,BRKS
      REAL*4            BRA1,BRK0,BRK0B,BRKS
      COMMON / TAUBRA / GAMPRT(500),JLIST(500),NCHAN
      IF (KTO.EQ.1) THEN
C     ==================
C AJWMOD: Set the BRs for (A1+ -> rho+ pi0) and (K*+ -> K0 pi+)
      BRA1 = PKORB(4,1)
      BRKS = PKORB(4,3)
      BRK0  = PKORB(4,5)
      BRK0B  = PKORB(4,6)
      ELSE
C     ====
C AJWMOD: Set the BRs for (A1+ -> rho+ pi0) and (K*+ -> K0 pi+)
      BRA1 = PKORB(4,2)
      BRKS = PKORB(4,4)
      BRK0  = PKORB(4,5)
      BRK0B  = PKORB(4,6)
      ENDIF
C     =====
      END


      SUBROUTINE INIPHX(XK00)
* ----------------------------------------------------------------------
*     INITIALISATION OF PARAMETERS
*     USED IN QED and/or GSW ROUTINES
* ----------------------------------------------------------------------
      COMMON / QEDPRM /ALFINV,ALFPI,XK0
      REAL*8           ALFINV,ALFPI,XK0
      REAL*8 PI8,XK00
*
      PI8    = 4.D0*DATAN(1.D0)
      ALFINV = 137.03604D0
      ALFPI  = 1D0/(ALFINV*PI8)
      XK0=XK00
      END

      SUBROUTINE INIMAS
C ----------------------------------------------------------------------
C     INITIALISATION OF MASSES
C
C     called by : KORALZ
C ----------------------------------------------------------------------
      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
*
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
C
C IN-COMING / OUT-GOING  FERMION MASSES
      AMTAU  = 1.7842
C --- tau mass must be the same as in the host program, what-so-ever
      AMTAU  = 1.777
      AMNUTA = 0.010
      AMEL   = 0.0005111
      AMNUE  = 0.0
      AMMU   = 0.105659 
      AMNUMU = 0.0
*
* MASSES USED IN TAU DECAYS

      AMPIZ  = 0.134964
      AMPI   = 0.139568
      AMRO   = 0.773
      GAMRO  = 0.145
*C    GAMRO  = 0.666
      AMA1   = 1.251
      GAMA1  = 0.599
      AMK    = 0.493667
      AMKZ   = 0.49772
      AMKST  = 0.8921
      GAMKST = 0.0513
C
C
C IN-COMING / OUT-GOING  FERMION MASSES
!!      AMNUTA = PKORB(1,2)
!!      AMNUE  = PKORB(1,4)
!!      AMNUMU = PKORB(1,6)
C
C MASSES USED IN TAU DECAYS  Cleo settings
!!      AMPIZ  = PKORB(1,7)
!!      AMPI   = PKORB(1,8)
!!      AMRO   = PKORB(1,9)
!!      GAMRO  = PKORB(2,9)
      AMA1   = 1.275   !! PKORB(1,10)
      GAMA1  = 0.615   !! PKORB(2,10)
!!      AMK    = PKORB(1,11)
!!      AMKZ   = PKORB(1,12)
!!      AMKST  = PKORB(1,13)
!!      GAMKST = PKORB(2,13)
C


      RETURN
      END
      subroutine bostdq(idir,vv,pp,q)
*     *******************************
c Boost along arbitrary vector v (see eg. J.D. Jacson, Classical 
c Electrodynamics).
c Four-vector pp is boosted from an actual frame to the rest frame 
c of the four-vector v (for idir=1) or back (for idir=-1). 
c q is a resulting four-vector.
c Note: v must be time-like, pp may be arbitrary.
c
c Written by: Wieslaw Placzek            date: 22.07.1994
c Last update: 3/29/95                     by: M.S.
c 
      implicit DOUBLE PRECISION (a-h,o-z)
      parameter (nout=6)
      DOUBLE PRECISION v(4),p(4),q(4),pp(4),vv(4)  
      save
!
      do 1 i=1,4
      v(i)=vv(i)
 1    p(i)=pp(i)
      amv=(v(4)**2-v(1)**2-v(2)**2-v(3)**2)
      if (amv.le.0d0) then
        write(6,*) 'bosstv: warning amv**2=',amv
      endif
      amv=sqrt(abs(amv))
      if (idir.eq.-1) then
        q(4)=( p(1)*v(1)+p(2)*v(2)+p(3)*v(3)+p(4)*v(4))/amv
        wsp =(q(4)+p(4))/(v(4)+amv)
      elseif (idir.eq.1) then
        q(4)=(-p(1)*v(1)-p(2)*v(2)-p(3)*v(3)+p(4)*v(4))/amv
        wsp =-(q(4)+p(4))/(v(4)+amv)
      else
        write(nout,*)' >>> boostv: wrong value of idir = ',idir
      endif
      q(1)=p(1)+wsp*v(1)
      q(2)=p(2)+wsp*v(2)
      q(3)=p(3)+wsp*v(3)
      end
        









