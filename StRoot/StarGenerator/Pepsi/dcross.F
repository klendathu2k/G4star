
C **********************************************************************

      FUNCTION DCROSS(V1,V2)
      
      IMPLICIT NONE
      
C...Differential cross-section dsigma/dv1dv2; v1=x, v2=Q2 or y or W2.
C...Used for numerical integration etc.
C...Note, non-zero result only for region defined by cuts through CUT.

*
* to avoid variable conflictions, a second keep element is necessary
* with the same common block name (see LPTOU2)
*
      COMMON /LEPTOU/ CUT(14),LST(40),PARL(30),
     &                X,Y,W2,Q2,U
      REAL CUT,PARL,X,Y,W2,Q2,U
      INTEGER LST
      SAVE /LEPTOU/

      COMMON /LINTER/ PARI(50),EWQC(2,2,8),QC(8),ZL(2,4),ZQ(2,8),PQ(17)
      REAL  PARI,EWQC,QC,ZL,ZQ,PQ
      SAVE /LINTER/

      COMMON /LINTRL/ PSAVE(3,4,5),KSAVE(4),XMIN,XMAX,YMIN,YMAX,
     &Q2MIN,Q2MAX,W2MIN,W2MAX,ILEP,INU,IG,IZ
      REAL PSAVE,XMIN,XMAX,YMIN,YMAX,Q2MIN,Q2MAX,W2MIN,W2MAX
      INTEGER KSAVE,ILEP,INU,IG,IZ
      SAVE /LINTRL/

      COMMON /LOPTIM/ OPTX(4),OPTY(4),OPTQ2(4),OPTW2(4),COMFAC
      REAL OPTX,OPTY,OPTQ2,OPTW2,COMFAC
      SAVE /LOPTIM/

      COMMON /LINTEG/ NTOT,NPASS
      INTEGER NTOT,NPASS
      SAVE /LINTEG/

      INTEGER  NLUPDM,NPLBUF
      PARAMETER  (NLUPDM=4000,NPLBUF=5)
      COMMON/LUJETS/N,K(NLUPDM,5),P(NLUPDM,NPLBUF),V(NLUPDM,5)
      INTEGER  N,K
      REAL  P,V
      SAVE /LUJETS/

      COMMON/LUDAT2/KCHG(500,3),PMAS(500,4),PARF(2000),VCKM(4,4)
      INTEGER  KCHG
      REAL  PMAS,PARF,VCKM
      SAVE /LUDAT2/


      INTEGER LST2
      REAL V1,V2,DCROSS,S,PM2,Q2LOW,Q2UPP,YLOW,YUPP,W2LOW,W2UPP

      DCROSS=0.
      NTOT=NTOT+1
C...Variable V1 is x, variable V2 is either Q**2, y or W**2
      X=V1
      IF(X.LT.XMIN.OR.X.GT.XMAX) RETURN
      S=PARL(21)
      PM2=PSAVE(3,2,5)**2
      IF(LST(31).EQ.1) THEN
        Q2=V2
        Y=Q2/(PARL(21)*X)
        W2=(1.-X)*Y*PARL(21)+PSAVE(3,2,5)**2
      ELSEIF(LST(31).EQ.2) THEN
        Y=V2
        Q2=Y*X*PARL(21)
        W2=(1.-X)*Y*PARL(21)+PSAVE(3,2,5)**2
      ELSEIF(LST(31).EQ.3) THEN
        W2=V2
        Y=(W2-PSAVE(3,2,5)**2)/((1.-X)*PARL(21))
        Q2=X*Y*PARL(21)
      ENDIF
      Q2LOW=MAX(Q2MIN,X*YMIN*S,(W2MIN-PM2)*X/(1.-X))
      Q2UPP=MIN(Q2MAX,X*YMAX*S,(W2MAX-PM2)*X/(1.-X))
      YLOW=MAX(YMIN,Q2MIN/(S*X),(W2MIN-PM2)/(S*(1.-X)))
      YUPP=MIN(YMAX,Q2MAX/(S*X),(W2MAX-PM2)/(S*(1.-X)))
      W2LOW=MAX(W2MIN,(1.-X)*YMIN*S+PM2,Q2MIN*(1.-X)/X+PM2)
      W2UPP=MIN(W2MAX,(1.-X)*YMAX*S+PM2,Q2MAX*(1.-X)/X+PM2)
      IF(Q2.LT.Q2LOW.OR.Q2.GT.Q2UPP) RETURN
      IF(Y.LT.YLOW.OR.Y.GT.YUPP) RETURN
      IF(W2.LT.W2LOW.OR.W2.GT.W2UPP) RETURN
      LST2=LST(2)
      LST(2)=-2
      CALL LEPTO
      LST(2)=LST2
      IF(LST(21).NE.0) RETURN
      NPASS=NPASS+1
      DCROSS=PARI(31)*PQ(17)*COMFAC

      RETURN
      END
