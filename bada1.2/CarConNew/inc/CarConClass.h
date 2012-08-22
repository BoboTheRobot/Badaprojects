/*
 * CarConClass.h
 *
 *  Created on: 16.8.2010
 *      Author: mrak
 */

#ifndef CARCONCLASS_H_
#define CARCONCLASS_H_

#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FIo.h>
#include <FSystem.h>
#include <FLocales.h>

using namespace Osp::Base;
using namespace Osp::Io;
using namespace Osp::Base::Collection;

struct CarData {
		String Brand, Model;
		int ID;
		int fueltype;
		int startkm;
		DateTime time;
		ArrayListT<int> fueltypesarray;
		String fueltypesstring;
		int curodometervalue;
		int year;
		String licence, vin, isurance;
};
struct FuelTypeData {
		String Caption;
		int ID;
};
struct LocationData {
		String Caption;
		int ID;
};

static const int CARCONDATATYPEOK = 1;
static const int CARCONDATATYPEMISSED = 2;
static const int CARCONDATATYPEPARTIAL = 3;
static const int CARCONDATATYPEMISSEDPARTIAL = 4;
static const int CARCONDATATYPEAFTERPARTIAL = 5;

struct CarConData {
		int ID, CarID, FuelType, Location, Type, StKm;
		double StL, Price;
		String Remark;
		DateTime time;
		DateTime timejustdate;
		int PreviusStKm;
		DateTime Previustime;
		int PreviusType;
		int NextStKm, NextTrip;
		DateTime Nexttime;
		int NextType;
		double NextStL;
		int NextRecId;
		double NextCon;
		double Con;
		int Trip;
		double PreviusCon;
		double AvgCon;
		double ConTolerance;
		String FuelTypeCaption, LocationCaption;
};

struct CarExpenseData {
	int ID, CarID;
	String Caption;
	double Price;
	String Remark;
	DateTime time;
	DateTime timejustdate;
	int Odometer;
	int katid;
	String katcaption;
};

struct CarExpenseKatData {
		String Caption;
		int ID;
};

struct TripData {
	int ID, CarID, StOseb;
	DateTime StartTime;
	int StartOdometer, StartLocation;
	DateTime EndTime;
	int EndOdometer, EndLocation;
	double PricePerL, CalcExpenses, RealExpenses;
	String Remark;
	String StartLocationCaption, EndLocationCaption;
};

static const int SETTINGSDISTANCEUNIT_KM = 1;
static const int SETTINGSDISTANCEUNIT_MILE = 2;
static const int SETTINGSVOLUMEUNIT_LITER = 1;
static const int SETTINGSVOLUMEUNIT_GALON = 2;
static const int SETTINGSCONTYPE_MPG = 1;
static const int SETTINGSCONTYPE_LKM = 2;
static const int SETTINGSCONTYPE_GMI = 3;
static const int SETTINGSCONTYPE_KML = 4;
static const int SETTINGSCONTYPE_KMG = 5;
static const int SETTINGSCONTYPE_MIL = 6;
static const int SETTINGSCAPTIONTYPE_VOLUME = 1;
static const int SETTINGSCAPTIONTYPE_DISTANCE = 2;
static const int SETTINGSCAPTIONTYPE_CON = 3;
static const int SETTINGSCAPTIONTYPE_CURRENCY = 4;

struct SettingsData {
	int distanceunit;
	int volumeunit;
	int contype;
	int contype2;
	String currencycountrycode;
	String distanceunitstr;
	String distanceunitfullstr;
	String volumeunitstr;
	String volumeunitfullstr;
	String contypestr;
	String contype2str;
	int avgcalctype;
};

class CarConClass {
public:
	CarConClass();
	virtual ~CarConClass();
private:
	Database * db_;
	DbEnumerator* pEnum_;
	DbStatement* pStmt_;

public:
	bool isfiststart;
	CarData SelectedCar;
	int GetLastSelectedID(void);
	bool SetLastSelectedID(int lastselectedid);
	bool GetCarData(int carid, CarData & data, bool readfueltypes=false);
	bool SaveCarData(CarData & data);
	bool DeleteCarData(int carid);
	bool GetCarDataListStart(void);
	bool GetCarDataListGetData(CarData & data);
	bool GetCarDataListEnd(void);
	bool GetFuelTypeDataListStart(int filtercarid=0, bool reversefilter=false);
	bool GetFuelTypeDataListGetData(FuelTypeData & data);
	bool GetFuelTypeDataListEnd(void);
	bool GetFuelTypeData(int itemid, FuelTypeData & data);
	bool SaveFuelTypeData(FuelTypeData & data);
	bool DeleteFuelTypeData(int itemid, bool justcheck=false);
	bool GetLocationDataListStart(void);
	bool GetLocationDataListStart(Osp::Base::String filtercaption, int limit=0);
	bool GetLocationDataListGetData(LocationData & data);
	bool GetLocationDataListEnd(void);
	bool GetLocationData(int itemid, LocationData & data);
	bool SaveLocationData(LocationData & data);
	bool DeleteLocationData(int itemid, bool justcheck=false);
	int SelectAddLocationData(Osp::Base::String caption);
	bool GetExpenseKatDataListStart(void);
	bool GetExpenseKatDataListGetData(CarExpenseKatData & data);
	bool GetExpenseKatDataListEnd(void);
	bool GetExpenseKatData(int itemid, CarExpenseKatData & data);
	bool SaveExpenseKatData(CarExpenseKatData & data);
	bool DeleteExpenseKatData(int itemid, bool justcheck=false);
	bool GetExpenseDataFilterListStart(int carid, DateTime filterfromdate, DateTime filtertodate);
	bool GetExpenseDataListStart(int carid);
	bool GetExpenseDataListGetData(CarExpenseData & data);
	bool GetExpenseDataListEnd(void);
	bool GetExpenseData(int itemid, CarExpenseData & data);
	bool SaveExpenseData(CarExpenseData & data);
	bool DeleteExpenseData(int itemid, bool justcheck=false);
	void GetSettingsDataGetCaptions(int type, int typeval, String & retstr, String & retstrfull);
	int GetSettingsDataGetDefaultCon(int volumeunit, int distanceunit);
	bool GetSettingsDataCheckConIfValid(int volumeunit, int distanceunit, int conunit);
	bool SaveSettingsData(SettingsData & data);
	bool GetSettingsData(SettingsData & data);
	bool GetCarConGetLastData(int carid, int fueltype, CarConData & data, bool calccon=false);
	bool GetCarConPrevNext(int itemid, int carid, Osp::Base::DateTime date, CarConData & data);
	bool GetCarConData(int itemid, CarConData & data);
	double GetCarConAvgSum(int carid);
	bool SaveCarConData(CarConData & data);
	bool DeleteCarConData(int itemid, bool justcheck=false);
	bool GetCarConDataListStart(int carid);
	bool GetCarConDataListStart(int carid, DateTime filterfromdate, DateTime filtertodate);
	bool GetCarConDataListGetData(CarConData & data);
	bool GetCarConDataListEnd(void);
	bool Create(int lastselectedid);
	double ParseFloat(String & tmps);
	int ParseInteger(String & tmps);
	bool GetTripDataListStart(int carid);
	bool GetTripDataListGetData(TripData & data);
	bool GetTripDataListEnd(void);
	bool GetTripData(int itemid, TripData & data);
	bool SaveTripData(TripData & data);
	bool DeleteTripData(int itemid, bool justcheck=false);

	void CloseDatabase();

	void ClearCountryLangCodeCache();
	void AddCountryLangCodeCache(Osp::Base::String langcountrycode, Osp::Base::String desc);
	bool GetCountryLangCodeStart();
	bool GetCountryLangCodeGetData( Osp::Base::String & langcountrycode, Osp::Base::String & desc);
	bool GetCountryLangCodeEnd(void);
	void RecalculateAllCarConData();

	Osp::Base::String DateToSQLlite(Osp::Base::DateTime date);
	Osp::Base::String DateToSQLlite(const DbStatement &pSt_, int colindex, Osp::Base::DateTime date);
	Osp::Base::DateTime DateFromSQLlite(Osp::Base::String date);
	Osp::Base::DateTime DateFromSQLlite(const DbEnumerator & pEn_, int colindex, Osp::Base::DateTime & result);
};

#endif /* CARCONCLASS_H_ */
