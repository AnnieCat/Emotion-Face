#include "pxcsensemanager.h"
#include "pxcemotion.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
using namespace std;

class Emotions
{
public: 
	Emotions(){}
	~Emotions(){}
	void setup();
	void update();
	void cleanup();

	bool iSeeYou;

private:
	PXCSenseManager *mSenseMgr;
	PXCFaceModule * faceModule;
	PXCFaceConfiguration *facec;
	PXCFaceData *fdata;
};

void Emotions::setup()
{
	mSenseMgr = PXCSenseManager::CreateInstance();
	if (!mSenseMgr)
		cout << "failed to create SDK Sense Manager" << endl;
		
	mSenseMgr->EnableFace();

	faceModule = mSenseMgr->QueryFace();
	facec = faceModule->CreateActiveConfiguration();
	PXCFaceConfiguration::ExpressionsConfiguration *expc = facec->QueryExpressions();
	expc->Enable();
	expc->EnableAllExpressions();
	
	facec->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR);
	facec->ApplyChanges();

	fdata = faceModule->CreateOutput();

	mSenseMgr->EnableEmotion();
	cout << "Initialized" << endl;

	mSenseMgr->Init();
}

void Emotions::update()
{
	int numFaces = 0;
	if (mSenseMgr->AcquireFrame(true) >= PXC_STATUS_NO_ERROR)
	{
		// Emotion Data
		PXCEmotion *emotionDet = mSenseMgr->QueryEmotion();
		PXCEmotion::EmotionData arrData[10];
		
		fdata->Update();
		
		int numFaces = fdata->QueryNumberOfDetectedFaces();
		
		for (int i = 0; i < numFaces; ++i)
		{
			// FaceData
			PXCFaceData::Face *face = fdata->QueryFaceByIndex(i);
			PXCFaceData::ExpressionsData *edata = face->QueryExpressions();

			emotionDet->QueryAllEmotionData(i, &arrData[0]);
			
			//Face Detection and Location
			if (arrData->rectangle.x > -1 && arrData->rectangle.y > -1)
			{
				//cout << arrData->rectangle.x << ", " << arrData->rectangle.y << endl;
				iSeeYou = true;
			}
			else
				iSeeYou = false;

# pragma region Expression Logic

			if (iSeeYou)
			{
				PXCFaceData::ExpressionsData::FaceExpressionResult smileScore;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_SMILE, &smileScore);

				PXCFaceData::ExpressionsData::FaceExpressionResult raiseLeftBrow;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_LEFT, &raiseLeftBrow);
				PXCFaceData::ExpressionsData::FaceExpressionResult raiseRightBrow;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_RIGHT, &raiseRightBrow);

				PXCFaceData::ExpressionsData::FaceExpressionResult eyeClosedLeft;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_EYES_CLOSED_LEFT, &eyeClosedLeft);
				PXCFaceData::ExpressionsData::FaceExpressionResult eyeClosedRight;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_EYES_CLOSED_RIGHT, &eyeClosedRight);

				PXCFaceData::ExpressionsData::FaceExpressionResult kiss;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_KISS, &kiss);

				PXCFaceData::ExpressionsData::FaceExpressionResult openMouth;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_MOUTH_OPEN, &openMouth);

				PXCFaceData::ExpressionsData::FaceExpressionResult tongueOut;
				edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_TONGUE_OUT, &tongueOut);


				if (smileScore.intensity > 80)
					cout << "smile back!" << endl;

				if (raiseLeftBrow.intensity > 80 && raiseRightBrow.intensity > 80)
					cout << "eyebrows up" << endl;
				if (raiseLeftBrow.intensity > 80 && raiseRightBrow.intensity < 80)
					cout << "eyebrow raised" << endl;
				if (raiseLeftBrow.intensity < 80 && raiseRightBrow.intensity > 80)
					cout << "eyebrow raised" << endl;

				if (eyeClosedLeft.intensity > 80 && eyeClosedRight.intensity > 80)
					cout << "eyes Closed" << endl;
				//else
					// eyes open

				if (kiss.intensity > 80)
					cout << "kissy face!" << endl;

				if (openMouth.intensity > 80)
					cout << "say Ahhhhh" << endl;

				if (tongueOut.intensity > 80)
					cout << "Stick Tongue Out" << endl;

			}

			//PXCFaceData::ExpressionsData::FaceExpressionResult score;
			//edata->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_KISS, &score);
			//cout << score.intensity << endl;

			

# pragma endregion //Expression Logic

#pragma region Emotion Logic

			/*bool emotionPresent = false;
			int epidx = -1; pxcI32 maxScoreE = -3; pxcF32 maxscoreI = 0;

			for (int i = 0; i < 7; i++)
			{
			if (arrData[i].evidence < maxScoreE) continue;
			if (arrData[i].intensity < maxscoreI) continue;
			maxScoreE = arrData[i].evidence;
			maxscoreI = arrData[i].intensity;
			epidx = i;

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}

			if (maxScoreE > -1)
			{
			std::string foundEmo = "";
			switch (arrData[epidx].eid)
			{
			case PXCEmotion::EMOTION_PRIMARY_ANGER:
			foundEmo = "Anger";
			break;
			case PXCEmotion::EMOTION_PRIMARY_CONTEMPT:
			foundEmo = "Contempt";
			break;
			case PXCEmotion::EMOTION_PRIMARY_DISGUST:
			foundEmo = "Disgust";
			break;
			case PXCEmotion::EMOTION_PRIMARY_FEAR:
			foundEmo = "Fear";
			break;
			case PXCEmotion::EMOTION_PRIMARY_JOY:
			foundEmo = "Joy";
			break;
			case PXCEmotion::EMOTION_PRIMARY_SADNESS:
			foundEmo = "Sadness";
			break;
			case PXCEmotion::EMOTION_PRIMARY_SURPRISE:
			foundEmo = "Surprise";
			break;
			case PXCEmotion::EMOTION_SENTIMENT_POSITIVE:
			foundEmo = "Positive";
			break;
			case PXCEmotion::EMOTION_SENTIMENT_NEGATIVE:
			foundEmo = "Negative";
			break;
			case PXCEmotion::EMOTION_SENTIMENT_NEUTRAL:
			foundEmo = "Neutral";
			break;

			}
			cout << "outstanding emotion = " << foundEmo << endl;
			}


			if (maxscoreI > 0.4)
			emotionPresent = true;

			if (emotionPresent)
			{
			//int spidx = -1;
			maxScoreE = -3; maxscoreI = 0;
			for (int i = 0; i < 7; i++)
			{
			if (arrData[i].evidence < maxScoreE) continue;
			if (arrData[i].intensity < maxscoreI) continue;
			maxScoreE = arrData[i].evidence;
			maxscoreI = arrData[i].intensity;
			//spidx = i;
			}
			}*/
#pragma endregion //Emotion Logic
		}
		
		
		numFaces = emotionDet->QueryNumFaces();

		const PXCCapture::Sample *sample = mSenseMgr->QueryEmotionSample();
	
		mSenseMgr->ReleaseFrame();
	}
}

void Emotions::cleanup()
{
	//release whatever data you've initiated
	mSenseMgr->Close();
}

int main()
{
	Emotions myEmotions;

	myEmotions.setup();
	
	while (true)
		myEmotions.update();
	myEmotions.cleanup();
}